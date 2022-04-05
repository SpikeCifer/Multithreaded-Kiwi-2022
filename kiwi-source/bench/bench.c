#include "bench.h"

/* Returns 0 if random key is needed, else returns 1 */
void inquire_random_key(int argc) { random_key_is_required = (argc == 5); }

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

long int get_threads_num(long int requests_num, int remaining_threads) 
{
	if (requests_num < remaining_threads)	// Use when number of requests is less than MAX_THREAD_NUM
		return requests_num; 				// Set number of threads equal to the user's request;

	return remaining_threads;
}

int identify_request(char* request)
{
	if (strcmp(request, "read") == 0)
		return READ_MODE;
	else if (strcmp(request, "write") == 0)
		return WRITE_MODE;
	else if (strcmp(request, "readwrite") == 0)
		return MIX_MODE;
	else 
		return UNSUPPORTED_MODE;
}

/* Function validates user given arguments,
 * prints data about the environment and 
 * returns the execution mode
 */
int init_program(int argc, char** argv)
{
	// Check that main arguments are valid
	int mode = identify_request(argv[1]);

	if (argc < 4 || mode == UNSUPPORTED_MODE) {
		fprintf(stderr, "Usage: db-bench <write | read | readwrite> <count> <threads> (key)\n");
		exit(1);
	}

	srand(time(NULL));

	_print_header(atoi(argv[2]));
	_print_environment();

	inquire_random_key(argc);
	return mode;
}

Constructor_args *prepare_constructor_data(long int total_requests, int remaining_threads, void *db_pointer)
{
	Constructor_args *args_p = malloc(sizeof(Constructor_args));
	args_p->thread_num = get_threads_num(total_requests, remaining_threads);
	args_p->requests_num = total_requests;
	args_p->db_pointer = db_pointer;
	return args_p;
}

int main(int argc, char** argv)
{
	int mode = init_program(argc, argv);
	long int total_requests = atoi(argv[2]);
	void *db_pointer = open_database();
	char *results_str = (char*)malloc(200*sizeof(char));

	int max_threads = atoi(argv[3]);

	switch (mode)
	{
		case READ_MODE:
		{
			pthread_t readers_constructor;
			if (pthread_create(&readers_constructor, NULL, create_readers,
				(void *) prepare_constructor_data(total_requests, max_threads, db_pointer)) != 0)
			{
				printf("Error while trying to create readers constructor\n");
				exit(0);
			}

			pthread_join(readers_constructor, (void **) &results_str);
			break;
		}

		case WRITE_MODE:
		{
			pthread_t writers_constructor;
			if(pthread_create(&writers_constructor, NULL, create_writers, 
				(void *) prepare_constructor_data(total_requests, max_threads, db_pointer)) != 0)
			{
				printf("Error while trying to create writers constructor\n");
				exit(0);
			}

			pthread_join(writers_constructor, (void **) &results_str);
			break;
		}

		case MIX_MODE:
		{			
			handle_mixed_requests(total_requests, max_threads, db_pointer, results_str);
			break;
		}

		default:
		{
			printf("Request mode not supported\n");
			exit(0);
		}
	}

	db_close(db_pointer);

	printf(LINE);
	printf("%s", results_str);
	printf(LINE);
	
	return 1;
}
