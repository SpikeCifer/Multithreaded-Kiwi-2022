add_executable(kiwi-bench bench.c kiwi.c)
target_link_libraries(kiwi-bench indexer pthread snappy)

# set(LIBS -lindexer -lpthread -lsnappy)
# set(SOURCES bench.c kiwi.c)
