#ifndef STO
#error "STO required to use set2.h"
#endif

#ifndef SET_H
#define SET_H 1

#if defined(SET_USE_RBTREE)

#  include "map2.h"

#  define SET_T                       MAP_T
#  define SET_ALLOC(hash, cmp)        MAP_ALLOC(hash, cmp)
#  define SET_FREE(map)               MAP_FREE(map)

#  define SET_CONTAINS(map, key)      MAP_CONTAINS(map, key)
#  define SET_INSERT(map, key)        MAP_INSERT(map, key, NULL)
#  define SET_REMOVE(map, key)        MAP_REMOVE(map, key)

#  define TMSET_CONTAINS(map, key)    TMMAP_CONTAINS(map, key)
#  define TMSET_INSERT(map, key)      TMMAP_INSERT(map, key, NULL)
#  define TMSET_REMOVE(map, key)      TMMAP_REMOVE(map, key)

#else

#  error "SET type is not specified"

#endif

#endif /* SET_H */