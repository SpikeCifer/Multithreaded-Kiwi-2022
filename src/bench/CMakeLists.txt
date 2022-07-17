add_executable(kiwi-bench bench.c kiwi.c)
target_link_libraries(kiwi-bench indexer pthread snappy)