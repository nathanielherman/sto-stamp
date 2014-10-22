#if defined(STO) && !defined(GTM)
#include "single.h"
#else
#include "tm.h"
template<class T> using Single = T;
#define TM_SINGLE_TRANS_READ(var) TM_SHARED_READ(var)
#define TM_SINGLE_TRANS_WRITE(var, val) TM_SHARED_WRITE(var, val)
#define TM_SINGLE_SIMPLE_READ(var) (var)
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var=val)
#endif

#define TM_SINGLE_TRANS_INCR(var, val) TM_SINGLE_TRANS_WRITE(var, \
    TM_SINGLE_TRANS_READ(var) + val)

template<class T>
T* alloc_array(int N){
  T* alloc_memory = new T[N];
  return alloc_memory;
}

template<class T>
void free_array(T* ptr){
  delete [] ptr;
}


