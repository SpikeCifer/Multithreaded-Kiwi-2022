#include "bench.h"

void* create_readers(void *arguments)
{
	Constructor_args *args = (Constructor_args *)arguments;
	char *results_str = malloc(200);

	pthread_t thread_ids[args->thread_num];

	long long start_time = get_ustime_sec();
	double total_cost = 0;
	long int total_found = 0;

	// Create Threads
	for (int i = 0; i < args->thread_num; i++) {
		Thread_info *thread_p = malloc(sizeof(Thread_info));

		thread_p->id = i;
		thread_p->load = args->requests_num/args->thread_num;
		thread_p->db_p = args->db_pointer;

		pthread_create(&thread_ids[i], NULL, _read_test, (void*) thread_p);
	}

	// Terminate Threads	
	for (int i = 0; i < args->thread_num; i++) {
		int* res;

		pthread_join(thread_ids[i], (void *) &res);
		
		total_found += *res;
	}

	total_cost = get_ustime_sec() - start_time;

	sprintf(results_str, "|Random-Read	(done:%ld, found:%ld): %.6f sec/op; %.1f reads /sec(estimated); cost:%.6f(sec)\n",
			args->requests_num, total_found,
			(double) (total_cost / args->requests_num),
			(double) (args->requests_num / total_cost),
			(double) total_cost);

	free(args);
	return results_str;
}

void* create_writers(void* arguments)
{
	Constructor_args *args = (Constructor_args *)arguments;
	char *results_str = (char*)malloc(200*sizeof(char));

	pthread_t thread_ids[args->thread_num];

	long long start_time = get_ustime_sec();
	
	// Create Threads
	for (int i = 0; i < args->thread_num; i++) {
		Thread_info *thread_p = malloc(sizeof(Thread_info));

		thread_p->id = i;
		thread_p->load = args->requests_num/args->thread_num;
		thread_p->db_p = args->db_pointer;

		pthread_create(&thread_ids[i], NULL, _write_test, (void *) thread_p);
	}

	// Terminate Threads
	for (int i = 0; i < args->thread_num; i++)
		pthread_join(thread_ids[i], NULL);
	
	double total_cost = get_ustime_sec() - start_time;

	sprintf(results_str, "|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost: %.6f(sec);\n",
			args->requests_num, (double)(total_cost/args->requests_num),
			(double) (args->requests_num/total_cost), total_cost);

	free(args);
	return results_str;
}

void handle_mixed_requests(long int total_requests, int max_threads, void* db_pointer, char* results)
{
	float read_percentage;
	printf("Please provide the Read percentage: ");
	scanf("%f", &read_percentage);

	char *readers_results = (char*)malloc(100*sizeof(char));
	char *writers_results = (char*)malloc(100*sizeof(char));

	long int reader_requests = total_requests * read_percentage/100;
	long int writer_requests = total_requests - reader_requests;

	pthread_t writers_id, readers_id;
	
	pthread_create(&writers_id, NULL, create_writers, (void *) prepare_constructor_data(writer_requests, max_threads/2, db_pointer));
	pthread_create(&readers_id, NULL, create_readers, (void *) prepare_constructor_data(reader_requests, max_threads/2, db_pointer));

	pthread_join(writers_id, (void**) &writers_results);
	pthread_join(readers_id, (void**) &readers_results);

	strcpy(results, writers_results);
	strcat(results, readers_results);
}