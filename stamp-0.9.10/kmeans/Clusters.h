#include "tm.h"

#if defined(STO) && !defined(GTM)
#define D
#include "single.h"
#endif

#define CACHE_LINE_SIZE 64

typedef struct _Cluster{
    int nfeatures;
    int centers_len;
    float centers[1];
} _Cluster;

inline unsigned get_cluster_size(int nfeatures) {
    return (sizeof(_Cluster) + (nfeatures - 1) * sizeof(float) + CACHE_LINE_SIZE - 1) & ~(size_t)(CACHE_LINE_SIZE - 1);
}

#ifdef D
class Cluster: public Single<_Cluster*> {
 public:
    void install(TransItem& item, Transaction& txn) override {
        _Cluster* cluster_ptr = v_.access();
        _Cluster* new_cluster_ptr = item.write_value<_Cluster*>();
        memcpy(cluster_ptr, new_cluster_ptr, get_cluster_size(cluster_ptr->nfeatures));
        vers_.set_version_unlock(txn.commit_tid());
        item.clear_needs_unlock();
    }
};
#else
#define Cluster _Cluster
#endif 

_Cluster *_alloc_cluster(int nfeatures);
#ifndef D 
#define TM_SINGLE_TRANS_READ(var) TM_SHARED_READ(var)
#define TM_SINGLE_TRANS_READ_F(var) TM_SHARED_READ_F(var)
#define TM_SINGLE_TRANS_WRITE(var, val) TM_SHARED_WRITE(var, val)
#define TM_SINGLE_TRANS_WRITE_F(var, val) TM_SHARED_WRITE_F(var, val)
#define TM_SINGLE_SIMPLE_READ(var) (var)
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var=val)
#endif
