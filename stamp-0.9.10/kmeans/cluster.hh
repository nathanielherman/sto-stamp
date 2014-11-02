#if defined(STO) && !defined(GTM)
#define D
#include "single.h"
#else
#include "tm.h"
template<class T> using Single = T;
#endif

typedef __declspec(align(32)) struct _Cluster{
		int nfeatures;
		int centers_len;
		float* centers;
} _Cluster;

#ifdef D 
class Cluster: public Single<_Cluster>{
#else
class Cluster{
		private: _Cluster cluster;
#endif
		public:
			  void init_seq(int nfeatures){
#ifdef D
						_Cluster cluster;
#endif
						cluster.nfeatures = nfeatures;
						cluster.centers_len = 0;
						cluster.centers = (float*)malloc(sizeof(float)*nfeatures);
						int j;
						for (j = 0; j < nfeatures; j++){
								cluster.centers[j] = 0.0;
						}
#ifdef D
						s_.set_value(cluster);
#endif
				}

				void trans_add_center(TM_ARGDECL float* feature){
#ifdef D
						_Cluster new_cluster = transRead(t);
						int j;
						for (j = 0; j < new_cluster.nfeatures; j++){
								new_cluster.centers[j] += feature[j];
						}
						new_cluster.centers_len += 1;
						transWrite(t, new_cluster);
#else
						int j;
						for (j = 0; j < cluster.nfeatures; j++){
								TM_SHARED_WRITE_F(cluster.centers[j],
												TM_SHARED_READ_F(cluster.centers[j]) + feature[j]);
						}
						TM_SHARED_WRITE(cluster.centers_len,
										TM_SHARED_READ(cluster.centers_len) + 1);
#endif
				}

				void reset_seq(float* center){
#ifdef D
						_Cluster cluster = s_.read_value();
#endif
						int j;
						for (j = 0; j < cluster.nfeatures; j++){
								if (cluster.centers_len > 0){
										center[j] = cluster.centers[j]/cluster.centers_len;
								}
								cluster.centers[j] = 0.0;
						}
						cluster.centers_len = 0;
#ifdef D
						s_.set_value(cluster);
#endif
				}

				void free_seq(){
#if !defined(D)
						free(cluster.centers);
#endif
				}
};

#ifdef D 
#else
#define TM_SINGLE_TRANS_READ(var) TM_SHARED_READ(var)
#define TM_SINGLE_TRANS_READ_F(var) TM_SHARED_READ_F(var)
#define TM_SINGLE_TRANS_WRITE(var, val) TM_SHARED_WRITE(var, val)
#define TM_SINGLE_TRANS_WRITE_F(var, val) TM_SHARED_WRITE_F(var, val)
#define TM_SINGLE_SIMPLE_READ(var) (var)
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var=val)
#endif

#define TM_CLUSTER_UPDATE_CENTER(cluster, feature) cluster.trans_add_center(TM_ARG feature)
#define TM_CLUSTER_RESET(new_cluster, cluster) new_cluster.reset_seq(cluster)
#define TM_CLUSTER_FREE(cluster) cluster.free_seq()
#define TM_CLUSTER_INIT(cluster, nfeatures) cluster.init_seq(nfeatures);

template<class T>
T* alloc_array(int N){
  T* alloc_memory = new T[N];
  return alloc_memory;
}

template<class T>
void free_array(T* ptr){
  delete [] ptr;
}


