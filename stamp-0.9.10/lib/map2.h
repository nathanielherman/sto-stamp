#ifndef STO
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
#include "sto/Hashtable.hh"
#define MAP_T Hashtable<void*, void*, 1000000>
#define TMMAP_CONTAINS(map, key) ({ void* val; bool ret = map->transGet(TM_ARG (void *)key, val); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMMAP_FIND(map, key) ({ void *val = NULL; map->transGet(TM_ARG (void *)key, val); /*TM_ARG_ALONE.check_reads();*/ val; })
#define TMMAP_INSERT(map, key, data) ({ auto ret = map->transInsert(TM_ARG (void *) key, (void *)data); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG (void *)key); })

// Preventing double-definition
#define MAP_ALLOC(hash, cmp) (new MAP_T())
#define MAP_FREE(map) (delete map)
// XXX: THIS IS PROBABLY HURTING PERF SOMEWHERE
#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#define MAP_CONTAINS(map, key) __TRANS_WRAP(TMMAP_CONTAINS(map, key), bool)
#define MAP_FIND(map, key) __TRANS_WRAP(TMMAP_FIND(map, key), void*)
#define MAP_INSERT(map, key, data) __TRANS_WRAP(TMMAP_INSERT(map, key, data), bool)
#define MAP_REMOVE(map, key) __TRANS_WRAP(TMMAP_REMOVE(map, key), bool)

#endif

// XXX: "parallel" region malloc is probably correct to use normal malloc
// but maybe we can get a perf win if we do some thread-local malloc??
#define PMAP_ALLOC(hash, cmp) MAP_ALLOC(hash, cmp)
#define PMAP_FREE(map) MAP_FREE(map)

#endif /* MAP_H */
