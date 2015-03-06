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
// use the STAMP hash table (basically just list)
#  include "hashtable.h"

#  define MAP_T                       hashtable_t
#  define MAP_ALLOC(hash, cmp)        hashtable_alloc(1000000, hash, cmp, 2, 2)
#  define MAP_FREE(map)               hashtable_free(map)
#  define MAP_CONTAINS(map, key)      hashtable_containsKey(map, (void*)(key))
#  define MAP_FIND(map, key)          hashtable_find(map, (void*)(key))
#  define MAP_INSERT(map, key, data)  hashtable_insert(map, (void*)(key), (void*)(data))
#  define MAP_REMOVE(map, key)        hashtable_remove(map, (void*)(key))
#  define TMMAP_CONTAINS(map, key)    TMhashtable_containsKey(TM_ARG  (map), (void*)(key))
#  define TMMAP_FIND(map, key)       TMhashtable_find(TM_ARG  (map), (void*)(key))
#  define TMMAP_INSERT(map, key, data)  TMhashtable_insert(TM_ARG  (map), (void*)(key), (void*)(data))
#  define TMMAP_REMOVE(map, key)        TMhashtable_remove(TM_ARG  (map), (void*)(key))

   /* TODO: The following two can be extended to support STO Hashtable or tree */
#  define PMAP_INSERT(map, key, data) MAP_INSERT(map, key, data)
#  define PMAP_REMOVE(map, key) MAP_REMOVE(map, key)

#else /* !MAP_USE_TREE */
//hashtable
#include "sto/Hashtable.hh"
#define MAP_T Hashtable<void*, void*, 100000>
#define TMMAP_CONTAINS(map, key) ({ void* val; bool ret = map->transGet(TM_ARG key, val); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMMAP_FIND(map, key) ({ void *val = NULL; map->transGet(TM_ARG key, val); /*TM_ARG_ALONE.check_reads();*/ val; })
#define TMMAP_INSERT(map, key, data) ({ auto ret = map->transInsert(TM_ARG key, data); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG key); })

// Preventing double-definition
#define MAP_ALLOC(hash, cmp) (new MAP_T())
#define MAP_FREE(map) (delete map)
#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#define MAP_CONTAINS(map, key) __TRANS_WRAP(TMMAP_CONTAINS(map, key), bool)
#define MAP_FIND(map, key) __TRANS_WRAP(TMMAP_FIND(map, key), void*)
#define MAP_INSERT(map, key, data) __TRANS_WRAP(TMMAP_INSERT(map, key, data), bool)
#define MAP_REMOVE(map, key) __TRANS_WRAP(TMMAP_REMOVE(map, key), bool)

#endif

#define PMAP_ALLOC(hash, cmp) MAP_ALLOC(hash, cmp)
#define PMAP_FREE(map) MAP_FREE(map)

#if 0
template <typename T>
inline T __trans_wrap(std::function<T(void)> f) {
  TM_BEGIN(); T ret = f(); TM_END(); return ret;
}
#endif

#endif /* MAP_H */
