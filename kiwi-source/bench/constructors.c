#include "bench.h"
// ------------------------------UTILS------------------------------
float get_read_percentage()
{
	int read_percentage = 0;
	do 
	{
		printf("Please provide the Read percentage [0-100]: ");
		scanf("%d", &read_percentage);
	} while(read_percentage < 0 || read_percentage > 100);

	return (float)read_percentage/100.0;
}

int share_threads(int max_threads, float read_percentage)
{
	if (max_threads == 2) // Can not devide them
		return 1;

	else if ((max_threads % 2) == 0) // Can be devided in two sizes
	{
		double temp_readers = (double)max_threads * read_percentage;
		printf("Readers: %f\n", temp_readers);

		if (temp_readers == (double)max_threads)
			return max_threads - 1;

		else if (temp_readers < 1.0) // Have at least 1 reader
			return 1;
		
		else if (temp_readers > max_threads - 1) // Have at least 1 writer
			return max_threads - 1;
		
		else if (temp_readers >= (int)temp_readers + 0.5) // If number has a decimal >= 0.5 increment by one
			return (int)temp_readers + 1;

		else if (temp_readers < (int)temp_readers + 0.5)
			return (int)temp_readers;
	}

	// if it gets here it's an odd number
	int res = share_threads(max_threads - 1, read_percentage);

	if(rand() % 10 >= 5) // 1 more for the writers
		return res--;
	else // 1 more for the readers
		return res++; 
}

// -------------------------- CONSTRUCTORS ------------------------------
void* create_readers(void *arguments)
{
	Constructor_args *args = (Constructor_args *)arguments;
	char *results_str = malloc(200);

	pthread_t thread_ids[args->thread_num];

	double total_cost = 0;
	long int total_found = 0;

	long long start_time = get_ustime_sec();

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
	char *readers_results = (char*)malloc(100*sizeof(char));
	char *writers_results = (char*)malloc(100*sizeof(char));

	float read_percentage = get_read_percentage();

	// No reads, only writes
	if (read_percentage == 0.0)
	{
		pthread_t writers_constructor;
		pthread_create(&writers_constructor, NULL, create_writers, 
			(void *) prepare_constructor_data(total_requests, max_threads, db_pointer));

		pthread_join(writers_constructor, (void **) &writers_results);
		strcpy(results, writers_results);
		return;
	}

	// No writes, only reads
	else if (read_percentage == 1.0)
	{
		pthread_t readers_constructor;
		pthread_create(&readers_constructor, NULL, create_readers,
			(void *) prepare_constructor_data(total_requests, max_threads, db_pointer));

		pthread_join(readers_constructor, (void **) &readers_results);
		strcpy(results, readers_results);
		return;
	}

	// The most expected scenario
	else
	{
		long int reader_requests = total_requests * read_percentage;
		long int writer_requests = total_requests - reader_requests;

		pthread_t writers_id, readers_id;

		int max_readers = share_threads(max_threads, read_percentage);
		int max_writers = max_threads - max_readers;

		if(pthread_create(&writers_id, NULL, create_writers, 
			(void *) prepare_constructor_data(writer_requests, max_writers, db_pointer)) != 0)
		{
			printf("Error while trying to create writers constructor");
			exit(0);
		}

		if(pthread_create(&readers_id, NULL, create_readers, 
			(void *) prepare_constructor_data(reader_requests, max_readers, db_pointer)) != 0)
		{
			printf("Error while trying to create readers constructor"); // PANIC MACRO COULD BE USED IN ANY OF THESE
			exit(0);
		}

		pthread_join(writers_id, (void**) &writers_results);
		pthread_join(readers_id, (void**) &readers_results);

		strcpy(results, writers_results);
		strcat(results, readers_results);
	}
}