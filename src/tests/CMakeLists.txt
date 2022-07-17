add_executable(
  bench_test
  bench/bench.cc
  ../bench/kiwi.c
)

target_link_libraries(
  bench_test
  GTest::gtest_main
  indexer pthread snappy
)

include(GoogleTest)
gtest_discover_tests(bench_test)