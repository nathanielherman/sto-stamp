#include <stdlib.h>
#include "Clusters.h"

unsigned get_cluster_size(int nfeatures){
		unsigned cluster_size = sizeof(_Cluster) + nfeatures * sizeof(float);
		cluster_size += (CACHE_LINE_SIZE - 1) - ((cluster_size - 1) % CACHE_LINE_SIZE);
		return cluster_size;
}

_Cluster *_alloc_cluster(int nfeatures){
		unsigned cluster_size = get_cluster_size(nfeatures);
		_Cluster *c = (_Cluster *)malloc(cluster_size);
		c->nfeatures = nfeatures;
		c->centers_len = 0;
		c->centers = (float *)((char *)c + sizeof(_Cluster));
		memset(c->centers, 0, nfeatures * sizeof(float));
		return c;
}

Cluster* alloc_cluster_seq(int nfeatures){
		_Cluster *_cluster = _alloc_cluster(nfeatures);
#ifdef D
		AlignedCluster *cluster = new AlignedCluster();
		cluster->data.write(_cluster);
		return &cluster->data;
#else
		return _cluster;
#endif
}

void reset_cluster_seq(Cluster* cluster, float* center){
#ifdef D
		_Cluster *_cluster = cluster->read();
#else
		_Cluster *_cluster = cluster;
#endif
		int j;
		for (j = 0; j < _cluster->nfeatures; j++){
				if (_cluster->centers_len > 0){
						center[j] = _cluster->centers[j]/_cluster->centers_len;
				}
				_cluster->centers[j] = 0.0;
		}
		_cluster->centers_len = 0;
#ifdef D
		cluster->write(_cluster);
#endif
}

void cluster_add_center(TM_ARGDECL Cluster *cluster, float* feature){
#ifdef D
		_Cluster *_cluster = cluster->transRead(TM_ARG_ALONE);
		_Cluster *new_cluster = _alloc_cluster(_cluster->nfeatures);
		int j;
		for (j = 0; j < new_cluster->nfeatures; j++){
				new_cluster->centers[j] = _cluster->centers[j] + feature[j];
		}
		new_cluster->centers_len = _cluster->centers_len + 1;
		cluster->transWrite(TM_ARG new_cluster);
#else
		int j;
		for (j = 0; j < cluster->nfeatures; j++){
				TM_SHARED_WRITE_F(cluster->centers[j],
								TM_SHARED_READ_F(cluster->centers[j]) + feature[j]);
		}
		TM_SHARED_WRITE(cluster->centers_len,
						TM_SHARED_READ(cluster->centers_len) + 1);
#endif
}

void free_cluster_seq(Cluster *cluster){
#ifdef D
		free(cluster->read());
#else
		free(cluster);
#endif
}
