/*  Program is used to run multiple executions of the kiwi program, 
 *  printing results for varying numbers of threads.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main ()
{

    for (int i = 1; i < 6; i++)
    {
        system("make clean");
        system("make all");
        
        printf("\n-----Running for %d threads-----\n", i*2);
        sleep(1);

        char input_str[] = "./kiwi-bench readwrite 100000 ";
        char thread_n = (i*2)+'0';
        strcat(input_str, &thread_n);
        system(input_str);
        sleep(5);
    }

    return 0;
}