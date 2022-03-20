#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

#define DATAS ("testdb")

void update_op_counter(int op_n, char* op)
{
	if ((op_n % 10000) == 0) {
			fprintf(stderr,"random %s finished %d ops%30s\r", 
					op, op_n, "\n");

			fflush(stderr);
		}
}

void request_key(char * key, int random_key_is_required, int op_n)
{
	if (random_key_is_required)
		_random_key(key, KSIZE);
	else
		snprintf(key, KSIZE, "key-%d", op_n);
}

void _write_test(long int count, int r)
{
	int i;
	double cost;
	long long start, end;
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

	for (i = 0; i < count; i++) {
		
		request_key(key, r, i);

		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(db, &sk, &sv);

		update_op_counter(i, "write");
	}
	
	end = get_ustime_sec();
	db_close(db);

	cost = end - start;

	printf(LINE);
	printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n",
		count, (double)(cost / count),
		(double)(count / cost),
		cost);	
}

void _read_test(long int count, int r)
{
	int i;
	double cost;
	long long start, end;
	Variant sk, sv;
	DB* db;

	char key[KSIZE + 1];
	int found = 0;

	db = db_open(DATAS);
	start = get_ustime_sec();

	for (i = 0; i < count; i++) {
		memset(key, 0, KSIZE + 1);

		request_key(key, r, i);

		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;

		if (db_get(db, &sk, &sv))
			found++;
		else
			INFO("not found key#%s", sk.mem);

		update_op_counter(i, "read");
	}

	end = get_ustime_sec();
	db_close(db);

	cost = end - start;

	printf(LINE);
	printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		count, found,
		(double)(cost / count),
		(double)(count / cost),
		cost);
}

void _mix_test(long int read_count, long int write_count, int r)
{
	printf("Umplimented mix function\n");
}
