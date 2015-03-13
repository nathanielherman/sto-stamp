#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <random>
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

void print_vector(std::vector<unsigned long>& v, bool error) {
    std::stringstream vecStr;
    vecStr << "{";
    for (auto it = v.begin(); it != v.end(); ++it) {
        vecStr << *it;
        if (it + 1 != v.end()) {
            vecStr << ", ";
        }
    }
    vecStr << "}" << std::endl;
    print_s(vecStr.str(), error);
}

void assert_equiv(list_t *tmlist, std::vector<unsigned long>& ref, std::string testname) {
    std::stringstream err;
    err << "Assertion failed in " << testname << ": ";
    if (list_getSize(tmlist) != ref.size()) {
        err << "size mismatch!" << std::endl;
        err << "Wanted " << ref.size() << ", got "
            << list_getSize(tmlist) << "!" << std::endl;
        std::cerr << err.str();
        abort();
    }
    list_iter_t it;
    list_iter_reset(&it, tmlist);

    for (auto rit = ref.begin(); rit != ref.end(); ++rit) {
        if (!list_iter_hasNext(&it, tmlist) ||
            *rit != (unsigned long)list_iter_next(&it, tmlist)) {
            err << "content mismatch at idx " << rit - ref.begin() << "!" << std::endl;
            std::cerr << err.str();
            print_list(tmlist, true);
            //print_vector(ref, true);
            abort();
        }
    }
}

void grow_list_random(list_t *tmlist, int tx_len, int rounds, std::vector<unsigned long>& refout) {
    std::default_random_engine gen;
    unsigned long dist_max = std::numeric_limits<unsigned long>::max();
    std::uniform_int_distribution<unsigned long> dist(0UL, dist_max);

    refout.clear();

    for (int i = 0; i < rounds; ++i) {
        for (int j = 0; j < tx_len; ++j) {
            refout.push_back(dist(gen));
        }

        TM_BEGIN();
        for (int j = 0; j < tx_len; ++j) {
            TMLIST_INSERT(tmlist, (void *)refout[i*tx_len + j]);
        }
        TM_END();
    }
}

void grow_list_monotonic(list_t *tmlist, int tx_len, unsigned long start, unsigned long num) {
    for (unsigned long i = 0; i < num; i += (unsigned long)tx_len) {
        TM_BEGIN();
        for (int j = 0; j < tx_len; ++j) {
            TMLIST_INSERT(tmlist, (void *)(start + i + (unsigned long)j));
        }
        TM_END();
    }
}

void scan_list(list_t *tmlist, int final_len) {
    list_iter_t it;
    int cur_len;
    unsigned long last, next;

    do {
        TM_BEGIN();
        cur_len = 0;
        last = 0;
        TMLIST_ITER_RESET(&it, tmlist);
        while (TMLIST_ITER_HASNEXT(&it, tmlist)) {
            next = (unsigned long)TMLIST_ITER_NEXT(&it, tmlist);
            if (last <= next) {
                last = next;
                ++cur_len;
            } else {
                std::stringstream err;
                err << "List scan failed! Incorrect element order at idx "
                    << cur_len << "!" << std::endl;
                std::cerr << err.str();
                abort();
            }
        }
        TM_END();
    } while (cur_len != final_len);
}

void shrink_list(list_t *tmlist, int tx_len, unsigned long high, int num) {
    int left = num;
    unsigned long loop_count = 0;
    while (left > 0) {
        TM_BEGIN();
        for (unsigned long i = 0; i < (unsigned long)tx_len; ++i) {
            TMLIST_REMOVE(tmlist, (void *)(high - loop_count*(unsigned long)tx_len - i));
        }
        TM_END();
        ++loop_count;
        left -= tx_len;
    }
}

int main() {
    std::vector<unsigned long> ref1, ref2;
    std::vector<unsigned long>::iterator last;
    std::thread *t1, *t2, *t3, *t4;

    // Test 1: Concurrent insertion
    std::cout << "Testing: concurrent insertion..." << std::endl;
    list_t *list = TMLIST_ALLOC(NULL);
    t1 = new std::thread(grow_list_random, list, 20, 1000, std::ref(ref1));
    t2 = new std::thread(grow_list_random, list, 20, 1000, std::ref(ref2));

    t1->join();
    t2->join();

    // Deduplicate reference list
    ref1.insert(ref1.end(), ref2.begin(), ref2.end());
    std::sort(ref1.begin(), ref1.end());
    last = std::unique(ref1.begin(), ref1.end());
    ref1.erase(last, ref1.end());
    
    assert_equiv(list, ref1, "Test 1");
    std::cout << "Passed." << std::endl;

    list_clear(list);
    ref1.clear();
    ref2.clear();
    delete t1;
    delete t2;

    // Test 2: Concurrent insert and traversal
    std::cout << "Testing: concurrent insert and traversal..." << std::endl;
    t1 = new std::thread(grow_list_monotonic, list, 100, 0UL, 50000UL);
    t2 = new std::thread(scan_list, list, 100000);
    t3 = new std::thread(grow_list_monotonic, list, 100, 50000UL, 50000UL);
    t4 = new std::thread(scan_list, list, 100000);

    t1->join();
    t2->join();
    t3->join();
    t4->join();

    for (unsigned long i = 0UL; i < 100000UL; ++i) {
        ref1.push_back(i);
    }

    assert_equiv(list, ref1, "Test 2");
    std::cout << "Passed." << std::endl;

    list_clear(list);
    ref1.clear();
    delete t1;
    delete t2;
    delete t3;
    delete t4;

    // Test 3: Concurrent remove and traversal
    std::cout << "Testing: concurrent remove and traversal..." << std::endl;
    for (unsigned long i = 0; i < 100000UL; ++i) {
        list_insert(list, (void *)i);
    }
    t1 = new std::thread(scan_list, list, 0);
    t2 = new std::thread(scan_list, list, 0);
    t3 = new std::thread(shrink_list, list, 100, 49999UL, 50000);
    t4 = new std::thread(shrink_list, list, 100, 99999UL, 50000);

    t1->join();
    t2->join();
    t3->join();
    t4->join();

    std::cout << "Passed." << std::endl;

    delete t1;
    delete t2;
    delete t3;
    delete t4;

    return 0;
}
