AUX_SOURCE_DIRECTORY(. SOURCES)
foreach(source ${SOURCES})
    message("Source File: ${source}")
endforeach()

add_library(kiwi-engine OBJECT ${SOURCES})

# Each of these files should be compiled into object files
# using the $(FINAL_CC) -fPIC -c $< command (Not sure which these args 
# are needed).