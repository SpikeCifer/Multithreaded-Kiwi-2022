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
/* Checks command line args and starts clock if all is OK */
void init_program(int argc, char** argv)
{
	// Check that main arguments are valid
	if (argc < 3 || function_is_not_supported(argv[1])) {
		fprintf(stderr,"Usage: db-bench <write | read> <count> (key)\n");
		exit(1);
	}

	srand(time(NULL));
}

void print_results(double cost, int found, int total_count)
{
	printf(LINE);
	printf("|Random-Read	(done:%d, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.6f(sec)\n",
		total_count, found,
		(double)(cost / total_count),
		(double)(total_count / cost),
		(double)cost);
}

int main(int argc, char** argv)
{
	double total_cost = 0.0;
	int total_found = 0;

	init_program(argc, argv);

	int thread_num = MAX_THREAD_NUM;
	Thread_results *thread_results;

	pthread_t tid[MAX_THREAD_NUM];
	long int total_count = atoi(argv[2]);

	if (total_count < MAX_THREAD_NUM)	// Use when number of requests is less than MAX_THREAD_NUM
		thread_num = total_count; 		// Set number of threads equal to the user's request;

	inquire_random_key(argc); 

	_print_header(total_count);
	_print_environment();

	if (strcmp(argv[1], "write") == 0)
		_write_test(total_count);

	else if (strcmp(argv[1], "read") == 0) {

		for (int i = 0; i < thread_num; i++) {
			Thread_info *thread_p = malloc(sizeof(Thread_info));
			thread_p->id = i;
			thread_p->load = total_count/thread_num;
			pthread_create(&tid[i], NULL, _read_test, (void*) thread_p);
		}
			
		for (int i = 0; i < thread_num; i++) {
			pthread_join(tid[i],(void *) &thread_results);
			total_cost = (double)thread_results->cost;
			total_found += thread_results->found;
		}

		print_results(total_cost, total_found, total_count);
		printf("cost: %.3f", total_cost);
	}
	
	else
		_mix_test(total_count, 0);

	return 1;
}
