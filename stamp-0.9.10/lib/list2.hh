#pragma once

#include "sto/Transaction.hh"
#include "sto/List.hh"

typedef List<void*, false, long (*)(const void*, const void*)> list_t;

typedef typename list_t::ListIter list_iter_t;

#define TMLIST_ITER_RESET(it, list) ({ TMlist_iter_reset(TM_ARG (it), (list)); /*TM_ARG_ALONE.check_reads();*/ })
#define TMLIST_ITER_HASNEXT(it, list) ({ bool ret = (it)->transHasNext(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ITER_NEXT(it, list) ({ auto ret = *(it)->transNext(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ALLOC(cmp) (new list_t(cmp))
#define TMLIST_FREE(list) /*TODO: (delete (list))*/
#define TMLIST_GETSIZE(list) ({ auto ret = (list)->transSize(TM_ARG_ALONE); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_ISEMPTY(list) (TMLIST_GETSIZE(list) == 0)
#define TMLIST_FIND(list, data) ({ auto ret = (list)->transFind(TM_ARG data); /*TM_ARG_ALONE.check_reads();*/ ret ? *ret : NULL; })
#define TMLIST_INSERT(list, data) ({ auto ret = (list)->transInsert(TM_ARG data); /*TM_ARG_ALONE.check_reads();*/ ret; })
#define TMLIST_REMOVE(list, data) (list)->transDelete(TM_ARG data)

#define list_alloc TMLIST_ALLOC
#define list_free TMLIST_FREE

#define __TRANS_WRAP(OP, TYPE) ({TYPE ___ret; TM_BEGIN(); ___ret = OP; TM_END(); ___ret;})
#define list_insert(list, data) __TRANS_WRAP(TMLIST_INSERT(list, data), bool)
#define list_iter_reset(it, list) ({ TM_BEGIN(); TMLIST_ITER_RESET(it, list); TM_END(); })
#define list_iter_hasNext(it, list) ({ bool ret; TM_BEGIN(); ret= (it)->transHasNext(TM_ARG_ALONE); TM_END(); ret; })
/*__TRANS_WRAP(TMLIST_ITER_HASNEXT(it, list), bool)*/
#define list_iter_next(it, list) __TRANS_WRAP(TMLIST_ITER_NEXT(it, list), void*)
#define list_getSize(list) __TRANS_WRAP(TMLIST_GETSIZE(list), size_t)

void TMlist_iter_reset(TM_ARGDECL list_iter_t* it, list_t* l);
