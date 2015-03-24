#pragma once

#include "sto/Queue.hh"
#include "random.h"
#include <random>
#include "queue_common.h"

#define queue_t Queue<void*, QUEUE_SIZE>

#define queue_alloc(c) (new queue_t)
#define queue_free(q) /*TODO*/
#define queue_push(q, d) ({ (q)->push((d)); true; })
#define queue_pop(q) (q)->pop()
// TODO: seed
#define queue_shuffle(q, r) ({ (q)->shuffle(std::default_random_engine(1)); })

#define TMQUEUE_ALLOC(c) queue_alloc(c)
#define TMQUEUE_FREE(q) /*TODO*/
#define TMQUEUE_ISEMPTY(q) ({ void *unused; !(q)->transFront(TM_ARG unused); })
#define TMQUEUE_PUSH(q, d) ({ (q)->transPush(TM_ARG (d)); true; })
#define TMQUEUE_POP(q) ({ void *ret = NULL; (q)->transFront(TM_ARG ret); (q)->transPop(TM_ARG_ALONE); ret; })
