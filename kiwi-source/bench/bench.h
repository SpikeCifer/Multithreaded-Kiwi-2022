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
#define THREADS_NUM (100)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

long long get_ustime_sec(void);
void _random_key(char *key,int length);

// Supported Functions
void _write_test(long int count, int r);
void _read_test(long int count, int r);
void _mix_test(long int read_count, long int write_count, int r);
