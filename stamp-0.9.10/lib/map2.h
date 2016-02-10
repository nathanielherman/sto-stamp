#if !defined(STO) && !defined(BOOSTING)
#error "STO required to use map2.h"
#endif

#ifndef MAP_H
#define MAP_H 1

#ifdef MAP_USE_RBTREE
#define MAP_USE_TREE
#endif

#undef MAP_USE_TREE


#ifdef MAP_USE_TREE
#include "sto/MassTrans.hh"
// masstree
#define MAP_T MassTrans<void*>

#define TMMAP_CONTAINS(map, key) ({ void* val; map->transGet(TM_ARG key, val); })
#define TMMAP_FIND(map, key) ({ void *val = NULL; map->transGet(TM_ARG key, val); val; })
#define TMMAP_INSERT(map, key, data) map->transInsert(TM_ARG key, data)
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG key); })

#elif defined(MAP_USE_HASHTABLE)
// XXX: if you really want to use the default hashtable, just include map.h instead of map2.h
#include "sto_hashtable.h"
// we do this so we can easily use the same methods for sets
#define MAP_T STOHASHTABLE_T
#define TMMAP_CONTAINS TMSTOHASHTABLE_CONTAINS
#define TMMAP_FIND TMSTOHASHTABLE_FIND
#define TMMAP_INSERT TMSTOHASHTABLE_INSERT
#define TMMAP_REMOVE TMSTOHASHTABLE_REMOVE

// Preventing double-definition
#define MAP_ALLOC STOHASHTABLE_ALLOC
#define MAP_FREE STOHASHTABLE_FREE
#define MAP_CONTAINS STOHASHTABLE_CONTAINS
#define MAP_FIND STOHASHTABLE_FIND
#define MAP_INSERT STOHASHTABLE_INSERT
#define MAP_REMOVE STOHASHTABLE_REMOVE

#endif

// XXX: "parallel" region malloc is probably correct to use normal malloc
// but maybe we can get a perf win if we do some thread-local malloc??
#define PMAP_ALLOC(hash, cmp) MAP_ALLOC(hash, cmp)
#define PMAP_FREE(map) MAP_FREE(map)

#endif /* MAP_H */
