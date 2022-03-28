#include "bench.h"

/* Checks if function is supported/valid. */
int function_is_not_supported(char* function) 
{	
	return (strcmp(function, "read") != 0 && 
			strcmp(function, "write") != 0 &&
			strcmp(function, "mix") != 0);
}

/* Returns 0 if random key is needed, else returns 1 */
void inquire_random_key(int argc) { random_key_is_required = (argc == 4); }

void _random_key(char *key,int length) 
{
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}

void init_program(int argc, char** argv)
{
	// Check that main arguments are valid
	if (argc < 3 || function_is_not_supported(argv[1])) {
		fprintf(stderr,"Usage: db-bench <write | read> <count> (key)\n");
		exit(1);
	}	

	srand(time(NULL));

	_print_header(atoi(argv[2]));
	_print_environment();

	inquire_random_key(argc);
}

long int get_threads_num(long int requests_num) 
{
	if (requests_num < MAX_THREAD_NUM)	// Use when number of requests is less than MAX_THREAD_NUM
		return requests_num; 		// Set number of threads equal to the user's request;

	return MAX_THREAD_NUM;
}

void write_requests(const int thread_num, const long int requests_num, void* db_pointer)
{
	pthread_t thread_ids[thread_num];

	// Create Threads
	for (int i = 0; i < thread_num; i++) {
		Thread_info *thread_p = malloc(sizeof(Thread_info));

		thread_p->id = i;
		thread_p->load = requests_num/thread_num;
		thread_p->db_p = db_pointer;

		pthread_create(&thread_ids[i], NULL, _write_test, (void *) thread_p);
	}

	// Terminate Threads
	for (int i = 0; i < thread_num; i++)
		pthread_join(thread_ids[i], NULL);
}

long int read_requests(const int thread_num, const long requests_num, void* db_pointer)
{
	pthread_t thread_ids[thread_num];

	long int total_found = 0;

	// Create Threads
	for (int i = 0; i < thread_num; i++) {
		Thread_info *thread_p = malloc(sizeof(Thread_info));

		thread_p->id = i;
		thread_p->load = requests_num/thread_num;
		thread_p->db_p = db_pointer;

		pthread_create(&thread_ids[i], NULL, _read_test, (void*) thread_p);
	}

	// Terminate Threads	
	for (int i = 0; i < thread_num; i++) {
		int* res;

		pthread_join(thread_ids[i], (void *) &res);
		
		total_found += *res;
	}

	return total_found;
}

int main(int argc, char** argv)
{
	init_program(argc, argv);

	long int total_count = atoi(argv[2]);
	long int thread_num = get_threads_num(total_count);

	void* db_pointer = open_database(); // It will be cast later to DB*

	if (strcmp(argv[1], "write") == 0) {

		// Time Execution
		long long start_time = get_ustime_sec();

		write_requests(thread_num, total_count, db_pointer);

		double total_cost = get_ustime_sec() - start_time;

		db_close(db_pointer);

		// Print Results
		printf(LINE);
		printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost: %.6f(sec);\n",
			total_count, (double)(total_cost/total_count),
			(double) (total_count/total_cost), total_cost);
	}
		
	else if (strcmp(argv[1], "read") == 0) {

		// Time Execution
		long long start_time = get_ustime_sec();

		long int total_found = read_requests(thread_num, total_count, db_pointer);

		double total_cost = get_ustime_sec() - start_time;

		db_close(db_pointer);

		// Print Results
		printf(LINE);
		printf("|Random-Read	(done:%ld, found:%ld): %.6f sec/op; %.1f reads /sec(estimated); cost:%.6f(sec)\n",
			total_count, total_found,
			(double) (total_cost / total_count),
			(double) (total_count / total_cost),
			(double) total_cost);
	}
	
	else
		_mix_test(total_count, 0);

	return 1;
}
