#include <stdlib.h>
#include "Clusters.h"

_Cluster *_alloc_cluster(int nfeatures){
    unsigned cluster_size = get_cluster_size(nfeatures);
    _Cluster *c = (_Cluster *)calloc(1, cluster_size);
    c->nfeatures = nfeatures;
    return c;
}
