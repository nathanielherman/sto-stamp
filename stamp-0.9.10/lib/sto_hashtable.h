#ifndef STO
#error "STO required to use sto_hashtable.h"
#endif

#pragma once

#include "types.h"
#include "sto/Hashtable.hh"

static inline long default_table_comparator(const void *a, const void* b) {
  return (long)a - (long)b;
}
static inline ulong_t default_hasher(const void *a) {
  return (ulong_t)a;
}

class __EqCompare {
 public:
  typedef long (*intcompare)(const void*, const void*);
 __EqCompare(intcompare c) : comp(c ? c : default_table_comparator) {}
  bool operator()(const void* a, const void* b) {
    return comp(a, b) == 0;
  }
  intcompare comp;
};

#define STOHASHTABLE_SIZE 1000000

#define STOHASHTABLE_T Hashtable<void*, void*, STOHASHTABLE_SIZE, ulong_t (*)(const void*), __EqCompare>
#define TMSTOHASHTABLE_CONTAINS(map, key) ({ void* val; bool ret = map->transGet(TM_ARG (void *)key, val); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMSTOHASHTABLE_FIND(map, key) ({ void *val = NULL; map->transGet(TM_ARG (void *)key, val); /*TM_ARG_ALONE.check_reads();*/ val; })
#define TMSTOHASHTABLE_INSERT(map, key, data) ({ auto ret = map->transInsert(TM_ARG (void *) key, (void *)data); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMSTOHASHTABLE_REMOVE(map, key) ({ map->transDelete(TM_ARG (void *)key); })

// Preventing double-definition
#define STOHASHTABLE_ALLOC(hash, cmp) (new STOHASHTABLE_T(STOHASHTABLE_SIZE, (hash) || (cmp) ? (hash) : default_hasher, __EqCompare(cmp)))
#define STOHASHTABLE_FREE(map) (delete map)
// XXX: THIS IS PROBABLY HURTING PERF SOMEWHERE
#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#define STOHASHTABLE_CONTAINS(map, key) __TRANS_WRAP(TMSTOHASHTABLE_CONTAINS(map, key), bool)
#define STOHASHTABLE_FIND(map, key) __TRANS_WRAP(TMSTOHASHTABLE_FIND(map, key), void*)
#define STOHASHTABLE_INSERT(map, key, data) __TRANS_WRAP(TMSTOHASHTABLE_INSERT(map, key, data), bool)
#define STOHASHTABLE_REMOVE(map, key) __TRANS_WRAP(TMSTOHASHTABLE_REMOVE(map, key), bool)
