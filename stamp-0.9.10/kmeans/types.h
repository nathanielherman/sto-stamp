#if !defined(GTM)
#include "single.h"
#else
#include "tm.h"
template<class T> using Single = T;
#define TM_SINGLE_TRANS_READ(var) TM_SHARED_READ(var)
#define TM_SINGLE_TRANS_WRITE(var, val) TM_SHARED_WRITE(var, val)
#define TM_SINGLE_SIMPLE_READ(var) (var)
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var=val)
#endif

