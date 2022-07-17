AUX_SOURCE_DIRECTORY(. SOURCES)
add_compile_options(-pedantic -D HASH_FUNCTION=HASH_FNV -Wall -g -ggdb) # This does not seem to work
add_library(indexer ${SOURCES}) # Although this could be a problem when adding new files, we will not be doing so, thus it's fine

# The following is the compile line arguments (removed a lot from the original)
# gcc -std=c99 -pedantic -D HASH_FUNCTION=HASH_FNV -Wall -g -ggdb -fPIC -c <.c file>