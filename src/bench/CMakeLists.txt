cmake_minimum_required(VERSION 3.23)
project(kiwi)

set(LIBS -lindexer -lpthread -lsnappy)
set(SOURCES bench.c kiwi.c)

add_executable(kiwi-bench ${SOURCES})
add_library(${LIBS})