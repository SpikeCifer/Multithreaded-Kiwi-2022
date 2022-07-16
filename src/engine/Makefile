CC = gcc
AR = ar
STD = -std=c99 -pedantic -DHASH_FUNCTION=HASH_FNV
DISWARN = -Wno-stringop-overflow -Wno-cpp -Wno-format-truncation -Wno-unused-but-set-variable -Wno-stringop-truncation -Wno-unused-variable
OPTIMIZATION ?= -O3 -fomit-frame-pointer -funroll-loops
DEBUG = -g -ggdb

FINAL_CFLAGS = $(STD) -Wall $(OPTIMIZATION) $(DEBUG) $(CFLAGS) $(DISWARN)

FINAL_CC = $(QUIET_CC)$(CC) $(FINAL_CFLAGS)
FINAL_AR = $(QUIET_AR)$(AR)

# Hide input into stder instead of showin in stdout
CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_AR = @printf '    %b %b\n' $(LINKCOLOR)AR$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

LIBINDEXER_OBJ = \
	db.o \
	memtable.o \
	indexer.o \
	sst.o \
	sst_builder.o \
	sst_loader.o \
	sst_block_builder.o \
	hash.o \
	bloom_builder.o \
	merger.o \
	compaction.o \
	skiplist.o \
	buffer.o \
	arena.o \
	utils.o \
	crc32.o \
	file.o \
	heap.o \
	vector.o \
	log.o \
	lru.o

# --------------------------- COMPILE ---------------------------
%.o: %.c
	$(FINAL_CC) -fPIC -c $<

libindexer.a: $(LIBINDEXER_OBJ)
	$(FINAL_AR) rcs libindexer.a $(LIBINDEXER_OBJ)

all: libindexer.a

.PHONY: all

# ------------------------------- CLEAN ---------------------------
clean:
	rm *.o libindexer.a 

.PHONY: clean

# ----------------------------- SAVE DEPENDENCIES ----------------------------
include Makefile.dep

dep:
	$(FINAL_CC) -MM *.c > Makefile.dep

.PHONY: dep