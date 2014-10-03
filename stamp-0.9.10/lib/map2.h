#ifdef MAP_USE_RBTREE
#define MAP_USE_TREE
#endif

#define MAP_USE_TREE


#ifdef MAP_USE_TREE
// masstree
#define MAP_T MassTrans<void*>
#define MAP_ALLOC(hash, cmp) (new MAP_T())
#define MAP_FREE(map) (delete map)

#define TMMAP_CONTAINS(map, key) ({ void* val; map->transGet(TM_ARG key, val); })
#define TMMAP_FIND(map, key) ({ void *val; map->transGet(TM_ARG key, val); val; })
#define TMMAP_INSERT(map, key, data) map->transInsert(TM_ARG key, data)
#define TMMAP_REMOVE(map, key) ({ map->transDelete(TM_ARG key); })

#else /* !MAP_USE_TREE */
//hashtable

#endif
