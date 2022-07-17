DEBUG=-g -ggdb
LIBRARIES = -lindexer -lpthread -lsnappy

# Look inside the engine folder for the libraries
all:
	gcc $(DEBUG) -Wall bench.c kiwi.c -L ../engine $(LIBRARIES) -o kiwi-bench
clean:
	rm -f kiwi-bench
	rm -rf testdb
