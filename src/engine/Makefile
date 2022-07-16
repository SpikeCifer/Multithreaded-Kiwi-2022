include defs.mk

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
	rm -rf *.o libindexer.a 

.PHONY: clean

# ----------------------------- SAVE DEPENDENCIES ----------------------------
include Makefile.dep

dep:
	$(FINAL_CC) -MM *.c > Makefile.dep

.PHONY: dep