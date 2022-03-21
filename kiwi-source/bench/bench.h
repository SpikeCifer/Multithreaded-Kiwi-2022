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
#define THREAD_NUM (100) //HERE

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

typedef struct Par{
    long int count; //HERE
    int r;
}Par;

long long get_ustime_sec(void);
void _random_key(char *key,int length);

void _write_test(long int count, int r);
void _read_test(void * pars);
void _mix_test(long int read_count, long int write_count, int r);
