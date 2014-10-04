#ifdef MAP_USE_RBTREE
#define MAP_USE_TREE
#endif

#undef MAP_USE_TREE


#ifdef MAP_USE_TREE
#include "sto/MassTrans.hh"
// masstree
#define MAP_T MassTrans<void*>

#define TMMAP_CONTAINS(map, key) ({ void* val; map->transGet(TM_ARG key, val); })
#define TMMAP_FIND(map, key) ({ void *val; map->transGet(TM_ARG key, val); val; })
#define TMMAP_INSERT(map, key, data) map->transInsert(TM_ARG key, data)
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG key); })

#else /* !MAP_USE_TREE */
//hashtable
#include "sto/Hashtable.hh"
#define MAP_T Hashtable<void*, void*>
#define TMMAP_CONTAINS(map, key) ({ void* val; map->transGet(TM_ARG key, val); })
#define TMMAP_FIND(map, key) ({ void *val; map->transGet(TM_ARG key, val); val; })
#define TMMAP_INSERT(map, key, data) map->transInsert(TM_ARG key, data)
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG key); })

#endif

#define MAP_ALLOC(hash, cmp) (new MAP_T())
#define MAP_FREE(map) (delete map)
#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#define MAP_CONTAINS(map, key) __TRANS_WRAP(TMMAP_CONTAINS(map, key), bool)
#define MAP_FIND(map, key) __TRANS_WRAP(TMMAP_FIND(map, key), void*)
#define MAP_INSERT(map, key, data) __TRANS_WRAP(TMMAP_INSERT(map, key, data), bool)
#define MAP_REMOVE(map, key) __TRANS_WRAP(TMMAP_REMOVE(map, key), bool)

#if 0
template <typename T>
inline T __trans_wrap(std::function<T(void)> f) {
  TM_BEGIN(); T ret = f(); TM_END(); return ret;
}
#endif
