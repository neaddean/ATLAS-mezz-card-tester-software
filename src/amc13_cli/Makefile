# Set the project home directory to the current directory
MY_PROJECT_ROOT = $(shell pwd)

# Include the headers in your project's include folder
INCLUDES = $(wildcard include/hcal/amc13/*.hh)

# The include path is your own include directory and the cactus includes
INCLUDE_PATH =  \
                -Iinclude 


# Set your library name here
AMC13_SOURCES = $(wildcard src/common/*.cc)
AMC13_OBJECT_FILES = $(patsubst src/common/%.cc,obj/%.o,${AMC13_SOURCES})

EXECUTABLE_SOURCES = $(wildcard src/common/*.cxx)
EXECUTABLE_OBJECT_FILES = $(patsubst src/common/%.cxx,obj/%.o,${EXECUTABLE_SOURCES})
EXECUTABLES = $(patsubst src/common/%.cxx,bin/%,${EXECUTABLE_SOURCES})

LIBRARY_PATH =  


LIBRARIES =     \
                -lpthread \
                \
                -lboost_filesystem \
                -lboost_regex \
                -lboost_system \
                -lboost_thread \
		\
		-lreadline -lcurses


CPP_FLAGS = -O3 -Wall -Werror -fPIC -g ${INCLUDE_PATH} -fno-strict-aliasing
LINK_LIBRARY_FLAGS = -fPIC -Wall -O3 ${LIBRARY_PATH} ${LIBRARIES}
LINK_EXECUTABLE_FLAGS = -Wall -g -ggdb -O3 ${LIBRARY_PATH} ${LIBRARIES} 

.PHONY: all _all build _buildall clean _cleanall

default: build

clean: _cleanall
_cleanall:
	rm -rf bin
	rm -rf obj

all: _all
build: _all
buildall: _all
_all: obj bin ${EXECUTABLES}

bin:
	mkdir -p bin

obj:
	mkdir -p obj


${EXECUTABLES}: bin/%: obj/%.o ${EXECUTABLE_OBJECT_FILES} ${AMC13_OBJECT_FILES}
	g++ ${LINK_EXECUTABLE_FLAGS}  $< ${AMC13_OBJECT_FILES} -o $@


${EXECUTABLE_OBJECT_FILES}: obj/%.o : src/common/%.cxx
	g++ -c ${CPP_FLAGS}  $< -o $@

${AMC13_OBJECT_FILES}: obj/%.o : src/common/%.cc
	g++ -c ${CPP_FLAGS}  $< -o $@


