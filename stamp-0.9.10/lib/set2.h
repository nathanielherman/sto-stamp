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
// XXX: better way to do this than just copying map2.h??

#include "sto/Hashtable.hh"
#define SET_T Hashtable<void*, void*, 1000000>
#define TMSET_CONTAINS(map, key) ({ void* val; bool ret = map->transGet(TM_ARG (void *)key, val); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMSET_INSERT(map, key) ({ auto ret = map->transInsert(TM_ARG (void *) key, (void*)NULL); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMSET_REMOVE(map, key) ({ map->transDelete(TM_ARG (void *)key); })

// Preventing double-definition
#define SET_ALLOC(hash, cmp) (new SET_T())
#define SET_FREE(map) (delete map)
#ifndef __TRANS_WRAP
#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#endif
#define SET_CONTAINS(map, key) __TRANS_WRAP(TMSET_CONTAINS(map, key), bool)
#define SET_INSERT(map, key) __TRANS_WRAP(TMSET_INSERT(map, key), bool)
#define SET_REMOVE(map, key) __TRANS_WRAP(TMSET_REMOVE(map, key), bool)

#undef __TRANS_WRAP

#else

#  error "SET type is not specified"

#endif

#endif /* SET_H */
