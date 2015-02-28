#include "tm.h"

#if defined(STO) && !defined(GTM)
#define D
#include "single.h"
#else
template<class T> using Single = T;
#endif

#define CACHE_LINE_SIZE 64

typedef struct _Cluster{
		int nfeatures;
		int centers_len;
		float centers[1];
} _Cluster;

unsigned get_cluster_size(int nfeatures);

#ifdef D 
class Cluster: public Single<_Cluster*>{
		public:
				void install(TransItem& item) {
						_Cluster* cluster_ptr = s_.read_value();
						_Cluster* new_cluster_ptr = item.template write_value<_Cluster*>();
						memcpy(cluster_ptr, new_cluster_ptr, get_cluster_size(cluster_ptr->nfeatures));
						free(new_cluster_ptr);
						Versioning::inc_version(s_.version());
					//	printf("cluter len %d\n", s_.read_value()->centers_len);
				}

};
#else
#define Cluster _Cluster
#endif 

template<typename T>
struct CacheLineStorage {
		public:
				 T data __attribute__ ((aligned (CACHE_LINE_SIZE)));
};

#define AlignedCluster CacheLineStorage<Cluster>

Cluster *alloc_cluster_seq(int nfeatures);
_Cluster *_alloc_cluster(int nfeatures);
void reset_cluster_seq(Cluster* cluster, float* center);
void cluster_add_center(TM_ARGDECL Cluster* cluster, float* feature);
void free_cluster_seq(Cluster* cluster);

#ifdef D 
#else
#define TM_SINGLE_TRANS_READ(var) TM_SHARED_READ(var)
#define TM_SINGLE_TRANS_READ_F(var) TM_SHARED_READ_F(var)
#define TM_SINGLE_TRANS_WRITE(var, val) TM_SHARED_WRITE(var, val)
#define TM_SINGLE_TRANS_WRITE_F(var, val) TM_SHARED_WRITE_F(var, val)
#define TM_SINGLE_SIMPLE_READ(var) (var)
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var=val)
#endif

#define TM_CLUSTER_UPDATE_CENTER(cluster, feature) cluster_add_center(TM_ARG cluster, feature)
#define TM_CLUSTER_RESET(new_cluster, cluster) reset_cluster_seq(new_cluster, cluster)
#define TM_CLUSTER_FREE(cluster) free_cluster_seq(cluster)
#define TM_CLUSTER_ALLOC(nfeatures) alloc_cluster_seq(nfeatures);
