CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17

GTEST_LIBS = -lgtest -lgtest_main -pthread

all: test_multimap cfs_sched

test_multimap: test_multimap.cc multimap.h
	$(CXX) $(CXXFLAGS) test_multimap.cc $(GTEST_LIBS) -o test_multimap

cfs_sched: cfs_sched.cc multimap.h
	$(CXX) $(CXXFLAGS) cfs_sched.cc -o cfs_sched

clean:
	rm -f test_multimap cfs_sched *.o
