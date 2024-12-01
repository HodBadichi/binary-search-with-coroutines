CC1 = clang++-16
CXXFLAGS1 = -O2 -std=c++20 -Wall
CC2 = /specific/disk1/hodbadihi/custom/gcc/build/bin/g++ 
CXXFLAGS2 = -fcoroutines -O2 -Wall

all: clang gcc

clang: binary-search.cpp utils.cpp main.cpp
	$(CC1) $(CXXFLAGS1) -o clang-binary-search $^

gcc: binary-search.cpp utils.cpp main.cpp
	$(CC2) $(CXXFLAGS2) -o gcc-binary-search $^

clean:
	rm -f clang-binary-search gcc-binary-search
