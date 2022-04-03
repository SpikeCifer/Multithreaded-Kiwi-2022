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

#define READ_MODE 0
#define WRITE_MODE 1
#define MIX_MODE 2
#define UNSUPPORTED_MODE 3

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

int random_key_is_required; // Flag defines whether program should gen random keys or not

typedef struct maker_info {
    int thread_num;
    long int requests_num;
    void *db_pointer;
} Constructor_args;

typedef struct thread_info{
    int id;
    long int load;
    void* db_p;
}Thread_info;

Constructor_args *prepare_constructor_data(long int total_requests, int remaining_threads, void *db_pointer);

long long get_ustime_sec(void);
void _random_key(char *key,int length);

void* create_writers(void *args);
void* create_readers(void *args);

void* _write_test(void *thread_w);
void* _read_test(void *thread_r);

void* open_database();
