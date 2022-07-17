#include <gtest/gtest.h>

extern "C" {
    #include "../../bench/bench.h"
}

TEST(bench_write, accepted)
{
    write_requests(100, 0);
}

TEST(bench_read, accepted)
{
    read_requests(100, 0);
}