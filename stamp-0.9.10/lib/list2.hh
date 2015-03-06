#pragma once

#ifndef STO
#error "STO required to use list2.hh"
#endif

#include "tm.h"
#include "sto/Transaction.hh"
#include "sto/List.hh"

#ifdef LIST_NO_DUPLICATES
typedef List<void*, false, long (*)(const void*, const void*)> list_t;
#else
typedef List<void*, true, long (*)(const void*, const void*)> list_t;
#endif

static inline long default_comparator(const void* a, const void* b) {
  return a < b ? -1 : a == b ? 0 : 1;
}

typedef typename list_t::ListIter list_iter_t;

#define TMLIST_ITER_RESET(it, list) ({ TMlist_iter_reset(TM_ARG (it), (list)); /*TM_ARG_ALONE.check_reads();*/ })
#define TMLIST_ITER_HASNEXT(it, list) ({ bool ret = (it)->transHasNext(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ITER_NEXT(it, list) ({ auto ret = *(it)->transNext(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ALLOC(cmp) (new list_t(cmp ? cmp : default_comparator))
#define TMLIST_FREE(list) /*TODO: (delete (list))*/
#define TMLIST_GETSIZE(list) ({ auto ret = (list)->transSize(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ISEMPTY(list) (TMLIST_GETSIZE(list) == 0)
#define TMLIST_FIND(list, data) ({ auto ret = (list)->transFind(TM_ARG data); /*TM_ARG_ALONE.check_reads();*/ ret ? *ret : NULL; })
#define TMLIST_INSERT(list, data) ({ auto ret = (list)->transInsert(TM_ARG data); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_REMOVE(list, data) (list)->transDelete(TM_ARG data)

#define list_alloc(cmp) TMLIST_ALLOC(cmp)
#define list_free(list) TMLIST_FREE(list)
#define list_insert(list, data) ((list)->insert((data)))
#define list_iter_hasNext(it, list) ((it)->hasNext())
#define list_iter_next(it, list) (*((it)->next()))
#define list_getSize(list) ((list)->size())
#define list_isEmpty(list) (((list)->size()) == 0)
#define list_find(list, data) ((list)->_find(data))
#define list_remove(list, data) ((list)->remove((data)))
#define list_clear(list) ((list)->clear())

#define PLIST_ALLOC(cmp) list_alloc(cmp)
#define PLIST_FREE(list) list_free(list)
#define PLIST_GETSIZE(list) list_getSize(list)
#define PLIST_INSERT(list, data) list_insert(list, data)
#define PLIST_REMOVE(list, data) list_remove(list, data)
#define PLIST_CLEAR(list) list_clear(list)

#define PLIST_FREE(list) TMLIST_FREE(list)
#define Plist_free PLIST_FREE

#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})

// Defined in thread.c
extern void TMlist_iter_reset(TM_ARGDECL list_iter_t* it, list_t* l);
extern void list_iter_reset(list_iter_t* it, list_t* l);
