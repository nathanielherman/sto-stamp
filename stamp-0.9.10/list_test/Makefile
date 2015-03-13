all:
	clang++ -std=c++11 -g -c -O3 -I../lib -DSTO -DLIST_NO_DUPLICATES list_test.cpp -o list_test.o
	clang++ -std=c++11 -g -c -O3 -I../lib -DSTO -DLIST_NO_DUPLICATES ../lib/thread.c -o ../lib/thread.o
	clang++ -std=c++11 -g -O3 ../lib/thread.o list_test.o -o list_test -lpthread

clean:
	rm -vf list_test list_test.o ../lib/thread.o
