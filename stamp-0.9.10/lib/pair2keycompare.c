#include "pair2keycompare.h"

/* Macro magickz. Some of our maps expect a key, value comparison 
 * function. But maps should really just take key comparison 
 * functions, so we convert a key comparison function to 
 * a key, value comparison function.
 */
#define REP5(F) F(0) F(1) F(2) F(3) F(4)

#define N_COMPAREFUNCS 5

#define KTH_COMPAREPAIRS_FUNC(i) long comparePairs_func ## i (const pair_t *p1, const pair_t *p2) { return compareKeys_funcs[i](p1->firstPtr, p2->firstPtr); }
#define COMPAREFUNC_ARRAY(i) comparePairs_func ## i ,

long (*compareKeys_funcs[N_COMPAREFUNCS]) (const void*, const void*);

REP5(KTH_COMPAREPAIRS_FUNC)

long (*comparePairs_funcs[N_COMPAREFUNCS]) (const pair_t*, const pair_t*) = { REP5(COMPAREFUNC_ARRAY) };

/* =============================================================================
 * compareDataPtrAddresses
 * -- Default compare function
 * =============================================================================
 */
static long
compareDataPtrAddresses (const void* a, const void* b)
{
  return ((long)a - (long)b);
}

comparePair_type get_comparePairsFunc(long (*compareKeys)(const void*, const void*)) {
  comparePair_type comparePairs = NULL;
  if (!compareKeys) {
    compareKeys = compareDataPtrAddresses;
  }
  int idx = 0;
  for (; idx < N_COMPAREFUNCS; ++idx) {
    if (!compareKeys_funcs[idx]) {
      compareKeys_funcs[idx] = compareKeys;
      comparePairs = comparePairs_funcs[idx];
      break;
    }
    else if (compareKeys_funcs[idx] == compareKeys) {
      comparePairs = comparePairs_funcs[idx];
      break;
    }
  }
  // make sure we didn't run out of functions
  assert(idx < N_COMPAREFUNCS);
  return comparePairs;
}
