#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

#define DATAS ("testdb")

void update_op_counter(int op_n, char* op) 
{
	if ((op_n % 1000) == 0 && op_n > 1) {
		fprintf(stderr,"Random %s finished %d ops%30s\r", 
				op, op_n, "");

		fflush(stderr);
	}
}

void request_random_key(char *key, int i)
{
	/* if you want to test random write, use the following */
		if (random_key_is_required)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
}

void* _write_test(void* thread_w)
{
	Thread_info *thread_par = (Thread_info*) thread_w;

	Variant sk, sv;

	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];

	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);

	long int offset = thread_par->id*thread_par->load;

	// Use DB
	for (int i = offset; i < offset + thread_par->load; i++) {

		request_random_key(key, i);

		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(thread_par->db_p, &sk, &sv);
		update_op_counter(i, "write");
	}
	
	free(thread_w);
	return NULL;
}

void* _read_test(void* thread_r)
{
	Thread_info *parameters = (Thread_info*) thread_r;

	int found = 0;

	Variant sk;
	Variant sv;

	char key[KSIZE + 1];

	long int offset = parameters->id * parameters->load;
	
	// Use the DB
	for (int i = offset; i < offset + parameters->load; i++) {
		memset(key, 0, KSIZE + 1);

		request_random_key(key, i);

		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;

		if (db_get((DB*)parameters->db_p, &sk, &sv)) {
			found++;
		} else {
			INFO("not found key#%s", 
					sk.mem);
    	}
		update_op_counter(i, "read");
	}

	free(thread_r);
	int* result = (int *)malloc(sizeof(int));
	*result = found;
	return (void *) result;
}

void* open_database(){
	database = db_open(DATAS);
	return database;
}

void _mix_test(long int read_count, long int write_count)
{
	printf("Unimplimented mix function");
}
