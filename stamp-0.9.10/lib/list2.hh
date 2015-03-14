#pragma once

#ifndef STO
#error "STO required to use list2.hh"
#endif

#include "pair.h"
#include "tm.h"
#include "sto/Transaction.hh"
#include "sto/List.hh"

static inline long default_comparator(const void* a, const void* b) {
  return a < b ? -1 : a == b ? 0 : 1;
}

class __ListCompare {
public:
  typedef long (*realcompare)(const void*, const void*);
  __ListCompare(realcompare c) : comp(c ? c : default_comparator) {}
  long operator()(const pair_t& a, const pair_t& b) {
    return comp(a.firstPtr, b.firstPtr);
  }
  realcompare comp;
};

#ifdef LIST_NO_DUPLICATES
typedef List<pair_t, false, __ListCompare> list_t;
#else
typedef List<pair_t, true, __ListCompare> list_t;
#endif

typedef typename list_t::ListIter list_iter_t;

#define TMLIST_ITER_RESET(it, list) ({ TMlist_iter_reset(TM_ARG (it), (list)); /*TM_ARG_ALONE.check_reads();*/ })
#define TMLIST_ITER_HASNEXT(it, list) ({ bool ret = (it)->transHasNext(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ITER_NEXT(it, list) ({ auto ret = ((it)->transNext(TM_ARG_ALONE))->firstPtr; /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ALLOC(cmp) (new list_t(cmp ? cmp : default_comparator))
#define TMLIST_FREE(list) /*TODO: (delete (list))*/
#define TMLIST_GETSIZE(list) ({ auto ret = (list)->transSize(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ISEMPTY(list) (TMLIST_GETSIZE(list) == 0)
#define TMLIST_FIND(list, data) ({ auto ret = (list)->transFind(TM_ARG (pair_t){data, NULL}); /*TM_ARG_ALONE.check_reads();*/ ret ? ret->firstPtr : NULL; })
#define TMLIST_INSERT(list, data) ({ auto ret = (list)->transInsert(TM_ARG (pair_t){data, NULL}); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_REMOVE(list, data) (list)->transDelete(TM_ARG (pair_t){data, NULL})

#define TMLIST_FULL_ITER_NEXT(it, list) ({ auto ret = (it)->transNext(TM_ARG_ALONE); *ret; })
#define TMLIST_FULL_FIND(list, data) ({ auto ret = (list)->transFind(TM_ARG (pair_t){data, NULL}); ret ? *ret : (pair_t){NULL, NULL}; })
#define TMLIST_FULL_INSERT(list, data, seconddata) ({ auto ret = (list)->transInsert(TM_ARG (pair_t){data, seconddata}); ret; })

#define list_full_iter_next(it, list) ({ auto ret = (it)->next(); *ret; })
#define list_full_find(list, data) ({ auto ret = (list)->find((pair_t){data, NULL}); ret ? *ret : (pair_t){NULL, NULL}; })
#define list_full_insert(list, data, seconddata) ({ auto ret = (list)->insert((pair_t){data, seconddata}); ret; })

#define list_alloc(cmp) TMLIST_ALLOC(cmp)
#define list_free(list) TMLIST_FREE(list)
#define list_insert(list, data) ((list)->insert(((pair_t){data, NULL})))
#define list_iter_hasNext(it, list) ((it)->hasNext())
#define list_iter_next(it, list) (((it)->next())->firstPtr)
#define list_getSize(list) ((list)->size())
#define list_isEmpty(list) (((list)->size()) == 0)
#define list_find(list, data) ({ auto ret = (list)->find((pair_t){(data), NULL}); ret ? ret->firstPtr : NULL; })
#define list_remove(list, data) ((list)->remove<false>((pair_t){(data), NULL}))
#define list_clear(list) ((list)->clear())

#define PLIST_ALLOC(cmp) list_alloc(cmp)
#define PLIST_FREE(list) list_free(list)
#define PLIST_GETSIZE(list) list_getSize(list)
#define PLIST_INSERT(list, data) list_insert(list, data)
#define PLIST_REMOVE(list, data) list_remove(list, data)
#define PLIST_CLEAR(list) list_clear(list)


#define Plist_free PLIST_FREE

// Defined in thread.c
extern void TMlist_iter_reset(TM_ARGDECL list_iter_t* it, list_t* l);
extern void list_iter_reset(list_iter_t* it, list_t* l);
