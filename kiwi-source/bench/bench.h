#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define KSIZE (16)
#define VSIZE (1000)
#define MAX_THREAD_NUM (10)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

int random_key_is_required; // Flag defines whether program should gen random keys or not

typedef struct thread_info{
    int id;
    long int load;
    void* db_p;
}Thread_info;

typedef struct thread_results{
    double cost;
    int found;
}Thread_results;

long long get_ustime_sec(void);
void _random_key(char *key,int length);

void* _write_test(void *thread_w);
void* _read_test(void *thread_r);
void _mix_test(long int read_count, long int write_count);

void* open_database();
