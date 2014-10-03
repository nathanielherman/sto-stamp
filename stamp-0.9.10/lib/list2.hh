#pragma once

#include "sto/Transaction.hh"
#include "sto/List.hh"

typedef List<void*> list_t;

typedef typename list_t::ListIter list_iter_t;

#define TMLIST_ITER_RESET(it, list) TMlist_iter_reset(TM_ARG it, list)
#define TMLIST_ITER_HASNEXT(it, list) it.hasNext(TM_ARG)
#define TMLIST_ITER_NEXT(it, list) it.next(TM_ARG)
#define TMLIST_ALLOC(cmp) (new list_t(cmp))
#define TMLIST_FREE(list) (delete list)
#define TMLIST_GETSIZE(list) list->transSize(TM_ARG)
#define TMLIST_ISEMPTY(list) (TMLIST_GETSIZE(list) == 0)
#define TMLIST_FIND(list, data) list->transFind(TM_ARG data)
#define TMLIST_INSERT(list, data) list->transInsert(TM_ARG data)
#define TMLIST_REMOVE(list, data) list->transRemove(TM_ARG data)


void TMlist_iter_reset(TM_ARGDECL list_iter_t* it, list_t* l) {
  if (!it->valid())
    *it = l->transIter(TM_ARG);
  else
    it->transReset();
}
