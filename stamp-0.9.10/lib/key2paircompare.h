//#ifndef KEY2PAIR_H
//#define KEY2PAIR_H 1

//#include "pair.h"

#define GEN_COMPAREPAIR_HEADER(keyCompare) long keyCompare ## _pairs (const pair_t*, const pair_t*)
#define GEN_COMPAREPAIR_BODY(keyCompare) long keyCompare ## _pairs (const pair_t *a, const pair_t *b) { return keyCompare(a->firstPtr, b->firstPtr); }

//#endif
