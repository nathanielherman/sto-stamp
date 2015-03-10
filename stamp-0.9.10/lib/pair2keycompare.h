#ifndef PAIR2KEY_H
#define PAIR2KEY_H

#include "pair.h"

typedef long (*comparePair_type)(const pair_t*, const pair_t*);
comparePair_type get_comparePairsFunc(long (*compareKeys)(const void*, const void*));

#endif
