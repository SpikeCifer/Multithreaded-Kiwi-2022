#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

#define DATAS ("testdb")

void update_op_counter(int op_n, char* op) {
	if ((op_n % 1000) == 0) {
			fprintf(stderr,"BLYAAAAAAAT random %s finished %d ops%30s\r", 
					op, op_n, 
					"");

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

void _write_test(long int load)
{
	int i;
	double cost;
	long long start,end;
	Variant sk, sv;
	DB* db;

	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];

	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);

	db = db_open(DATAS);
	start = get_ustime_sec();

	// Use DB
	for (i = 0; i < load; i++) {

		request_random_key(key, i);

		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(db, &sk, &sv);
		update_op_counter(i, "write");
	}

	db_close(db);

	// Calculate Cost
	end = get_ustime_sec();
	cost = end - start;

	// Show Results
	printf(LINE);
	printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n",
			load, (double)(cost / load),
			(double)(load / cost) ,cost);	
}

void* _read_test(void* thread_p)
{
	Thread_info *parameters = (Thread_info*) thread_p;

	int ret;
	int found = 0;
	double cost;
	long long start,end;
	Variant sk;
	Variant sv;
	DB* db;
	char key[KSIZE + 1];

	long int offset = parameters->id*parameters->load;

	db = db_open(DATAS);
	start = get_ustime_sec();
	
	// Use the DB
	for (int i = offset; i < offset + parameters->load; i++) {
		memset(key, 0, KSIZE + 1);

		request_random_key(key, i);

		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;
		ret = db_get(db, &sk, &sv);

		if (ret) {
			found++;
		} else {
			INFO("not found key#%s", 
					sk.mem);
    	}

		update_op_counter(i, "read");
	}

	db_close(db);

	end = get_ustime_sec();
	cost = end - start;

	printf(LINE);
	printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		parameters->load, found,
		(double)(cost / parameters->load),
		(double)(parameters->load / cost),
		cost);

	free(thread_p);
	return NULL;
}

void _mix_test(long int read_count, long int write_count)
{
	printf("Umplimented mix function");
}
