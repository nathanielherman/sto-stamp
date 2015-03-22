#ifndef STO
#error "STO required to use set2.h"
#endif

#ifndef SET_H
#define SET_H 1

#if defined(SET_USE_MAP)

#  include "map2.h"

#  define SET_T                       MAP_T
#  define SET_ALLOC(hash, cmp)        MAP_ALLOC(hash, (long int (*)(const pair_t*, const pair_t*))(cmp))
#  define SET_FREE(map)               MAP_FREE(map)

#  define SET_CONTAINS(map, key)      MAP_CONTAINS(map, key)
#  define SET_INSERT(map, key)        MAP_INSERT(map, key, NULL)
#  define SET_REMOVE(map, key)        MAP_REMOVE(map, key)

#  define TMSET_CONTAINS(map, key)    TMMAP_CONTAINS(map, key)
#  define TMSET_INSERT(map, key)      TMMAP_INSERT(map, key, NULL)
#  define TMSET_REMOVE(map, key)      TMMAP_REMOVE(map, key)

#elif defined(SET_USE_HASHTABLE)
#include "sto_hashtable.h"
#define SET_T STOHASHTABLE_T
#define TMSET_CONTAINS TMSTOHASHTABLE_CONTAINS
#define TMSET_INSERT(map, key) TMSTOHASHTABLE_INSERT(map, key, NULL)
#define TMSET_REMOVE TMSTOHASHTABLE_REMOVE

// Preventing double-definition
#define SET_ALLOC STOHASHTABLE_ALLOC
#define SET_FREE STOHASHTABLE_FREE
#define SET_CONTAINS STOHASHTABLE_CONTAINS
#define SET_INSERT(map, key) STOHASHTABLE_INSERT(map, key, NULL)
#define SET_REMOVE STOHASHTABLE_REMOVE

#else

#  error "SET type is not specified"

#endif

#endif /* SET_H */
