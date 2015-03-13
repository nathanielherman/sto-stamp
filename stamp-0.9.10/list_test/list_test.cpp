#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <random>
#include "list2.hh"

void print_list(list_t *list) {
    list_iter_t it;
    std::cout << "{";
    list_iter_reset(&it, list);
    while (list_iter_hasNext(&it, list)) {
        std::cout << (unsigned long)(list_iter_next(&it, list));
        if (list_iter_hasNext(&it, list)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;
}

void assert_equiv(list_t *tmlist, std::vector<unsigned long>& ref, std::string testname) {
    std::stringstream err;
    err << "Assertion failed in " << testname << ": ";
    if (list_getSize(tmlist) == ref.size()) {
        err << "size mismatch!" << std::endl;
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
            print_list(tmlist);
            abort();
        }
    }
}

void grow_list(list_t *tmlist, int tx_len, int rounds, std::vector<unsigned long>& refout) {
    unsigned long n;
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
            TMLIST_INSERT(tmlist, (void *)refout[j]);
        }
        TM_END();
    }
}

int main() {
    std::vector<unsigned long> ref1, ref2;
    std::thread *t1, *t2;
    list_t *list = TMLIST_ALLOC(NULL);

    // Test 1: Concurrent insertion
    t1 = new std::thread(grow_list, list, 10, 100, std::ref(ref1));
    t2 = new std::thread(grow_list, list, 10, 100, std::ref(ref2));

    t1->join();
    t2->join();

    ref1.insert(ref1.end(), ref2.begin(), ref2.end());
    std::sort(ref1.begin(), ref1.end());
    assert_equiv(list, ref1, "Test 1");

    TMLIST_FREE(list)
    ref1.clear();
    ref2.clear();
    delete t1;
    delete t2;

    // More tests here

    return 0;
}
