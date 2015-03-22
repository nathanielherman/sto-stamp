#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include "list2.hh"

inline void print_s(const std::string& msg, bool error) {
    if (error) {
        std::cerr << msg;
    } else {
        std::cout << msg;
    }
}

void print_list(list_t *list, bool error) {
    list_iter_t it;
    std::stringstream liststr;
    liststr << "{";
    list_iter_reset(&it, list);
    while (list_iter_hasNext(&it, list)) {
        liststr << (unsigned long)(list_iter_next(&it, list));
        if (list_iter_hasNext(&it, list)) {
            liststr << ", ";
        }
    }
    liststr << "}" << std::endl;
    print_s(liststr.str(), error);
}

#define TMLIST_PRINT(list) {                                                                   \
								list_iter_t it;                                                \
								std::stringstream liststr;                                     \
								liststr << "{";                                                \
								TMLIST_ITER_RESET(&it, list);                                  \
								while (TMLIST_ITER_HASNEXT(&it, list)) {                       \
									liststr << (unsigned long)(TMLIST_ITER_NEXT(&it, list));   \
									if (TMLIST_ITER_HASNEXT(&it, list)) {                      \
										liststr << ", ";                                       \
									}                                                          \
								}                                                              \
								liststr << "}" << std::endl;                                   \
								print_s(liststr.str(), false);                                 \
							}

namespace TestFindInsertRemoveOpacity {
	std::mutex mu;
	std::condition_variable cv;
	std::string test_name = "FindInsertRemoveOpacity";
	bool tx1_ready = false;
	bool tx2_ready = false;
	
	void tx_thread1(list_t *tmlist) {
		TM_BEGIN();
		std::cout << "===TX1 START===" << std::endl;
		std::unique_lock<std::mutex> lk(mu);
		// 2UL should be present in the initial list
		bool found  = TMLIST_FIND(tmlist, (void *)2UL);
		if (found) {
			tx1_ready = true;
			cv.notify_one();

			TMLIST_PRINT(tmlist);

			std::cout << "tx1 waiting for tx2 to add element..." << std::endl;

			cv.wait(lk, []{return tx2_ready;});

			std::cout << "tx1 continues..." << std::endl;

			TMLIST_PRINT(tmlist);

			// the assert should never be triggered because of opacity
			bool status = TMLIST_REMOVE(tmlist, (void *)2UL);
			assert(status);

			std::cout << "tx1: item removed, exit" << std::endl;
		} else {
			std::cout << "tx1: item not found in list, exit." << std::endl;
		}

		TM_END();
		std::cout << "===TX1 COMMITTED===" << std::endl;
	}

	void tx_thread2(list_t *tmlist) {
		TM_BEGIN();
		std::cout << "===TX2 START===" << std::endl;
		std::unique_lock<std::mutex> lk(mu);

		cv.wait(lk, []{return tx1_ready;});

		bool found = TMLIST_FIND(tmlist, (void *)2UL);
		assert(found);

		std::cout << "tx2 inserting element into the list..." << std::endl;

		bool status = TMLIST_REMOVE(tmlist, (void *)1UL);
		assert(status);
		status = TMLIST_REMOVE(tmlist, (void *)3UL);
		assert(status);
		status = TMLIST_INSERT(tmlist, (void *)4UL);
		assert(status);

		tx2_ready = true;
		cv.notify_one();

		std::cout << "tx2: insert complete, exit." << std::endl;

		TM_END();
		std::cout << "===TX2 COMMITTED===" << std::endl;
	}

	void run_test() {
		std::cout << "Starting test \"" << test_name << "\"..." << std::endl;
		list_t *list = list_alloc(NULL);

		for (unsigned long i = 1; i <= 3; ++i) {
			list_insert(list, (void *)i);
		}

		std::thread t1(tx_thread1, list);
		std::thread t2(tx_thread2, list);

		t1.join();
		t2.join();

		print_list(list, false);

		std::cout << "End of test \"" << test_name << "\"." << std::endl;

		list_clear(list);

		return;
	}
}

int main() {
	TestFindInsertRemoveOpacity::run_test();
	return 0;
}