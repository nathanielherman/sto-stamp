/* =============================================================================
 *
 * normal.c
 * -- Implementation of normal k-means clustering algorithm
 *
 * =============================================================================
 *
 * Author:
 *
 * Wei-keng Liao
 * ECE Department, Northwestern University
 * email: wkliao@ece.northwestern.edu
 *
 *
 * Edited by:
 *
 * Jay Pisharath
 * Northwestern University.
 *
 * Chi Cao Minh
 * Stanford University
 *
 * =============================================================================
 *
 * For the license of bayes/sort.h and bayes/sort.c, please see the header
 * of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of kmeans, please see kmeans/LICENSE.kmeans
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of ssca2, please see ssca2/COPYRIGHT
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/mt19937ar.c and lib/mt19937ar.h, please see the
 * header of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/rbtree.h and lib/rbtree.c, please see
 * lib/LEGALNOTICE.rbtree and lib/LICENSE.rbtree
 * 
 * ------------------------------------------------------------------------
 * 
 * Unless otherwise noted, the following license applies to STAMP files:
 * 
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 * 
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
		* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
		* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	* THE POSSIBILITY OF SUCH DAMAGE.
	*
	* =============================================================================
	*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "common.h"
#include "normal.h"
#include "random.h"
#include "thread.h"
#include "timer.h"
#include "tm.h"
#include "util.h"

#define GTM 0//use GenericSTM or more specific data structures.
#if !defined(STO) 
#undef GTM 
#define GTM 1
#endif

#if STO && (!GTM)
#include "sto/SingleElem.hh"
#endif
	double global_time = 0.0;

	typedef struct args {
		float** feature;
		int     nfeatures;
		int     npoints;
		int     nclusters;
		int*    membership;
		float** clusters;
#if GTM
	int**   new_centers_len;
	float** new_centers;
#else
	SingleElem<int>** new_centers_len;
	SingleElem<float>** new_centers;
#endif	
	} args_t;

#if GTM
float global_delta;
long global_i; /* index into task queue */
#else
SingleElem<float> global_delta;
SingleElem<long> global_i;
#endif

#define CHUNK 3

/* =============================================================================
 * work
 * =============================================================================
 */
	static void
work (void* argPtr)
{
	TM_THREAD_ENTER();

	args_t* args = (args_t*)argPtr;
	float** feature         = args->feature;
	int     nfeatures       = args->nfeatures;
	int     npoints         = args->npoints;
	int     nclusters       = args->nclusters;
	int*    membership      = args->membership;
	float** clusters        = args->clusters;
#if GTM
	int**   new_centers_len = args->new_centers_len;
	float** new_centers     = args->new_centers;
#else
	SingleElem<int>** new_centers_len = args->new_centers_len;
	SingleElem<float>** new_centers = args->new_centers;
#endif	

	float delta = 0.0;
	int index;
	int i;
	int j;
	int start;
	int stop;
	int myId;

	myId = thread_getId();

	start = myId * CHUNK;

	while (start < npoints) {
		stop = (((start + CHUNK) < npoints) ? (start + CHUNK) : npoints);
		for (i = start; i < stop; i++) {
			printf("cluster %d start\n", i);

			index = common_findNearestPoint(feature[i],
					nfeatures,
					clusters,
					nclusters);
			/*
			 * If membership changes, increase delta by 1.
			 * membership[i] cannot be changed by other threads
			 */
			if (membership[i] != index) {
				delta += 1.0;
			}

			/* Assign the membership to object i */
			/* membership[i] can't be changed by other thread */
			membership[i] = index;

			/* Update new cluster centers : sum of objects located within */
			TM_BEGIN();
#if GTM
			TM_SHARED_WRITE(*new_centers_len[index],
					TM_SHARED_READ(*new_centers_len[index]) + 1);
#else
			new_centers_len[index]->transWrite(__transaction,
					new_centers_len[index]->transRead(__transaction) + 1);

#endif
			for (j = 0; j < nfeatures; j++) {
#if GTM
				TM_SHARED_WRITE_F(
						new_centers[index][j],
						(TM_SHARED_READ_F(new_centers[index][j]) + feature[i][j])
						);
#else
				new_centers[index][j].transWrite(__transaction,
						new_centers[index][j].transRead(__transaction) + feature[i][j]
						);
#endif
			}
		printf("before commit\n");
			TM_END();
		}
		printf("update task queue\n");

		/* Update task queue */
		if (start + CHUNK < npoints) {
			TM_BEGIN();
#if GTM
			start = (int)TM_SHARED_READ(global_i);
			TM_SHARED_WRITE(global_i, (start + CHUNK));
#else
			start = (int)global_i.transRead(__transaction);
			global_i.transWrite(__transaction, (start + CHUNK));
#endif
			TM_END();
		} else {
			break;
		}
	}
	printf("end update\n");

	TM_BEGIN();
#if GTM
	TM_SHARED_WRITE_F(global_delta, TM_SHARED_READ_F(global_delta) + delta);
#else
	global_delta.transWrite(__transaction, 
			global_delta.transRead(__transaction) + delta);
#endif

	TM_END();

	TM_THREAD_EXIT();
}


/* =============================================================================
 * normal_exec
 * =============================================================================
 */
	float**
normal_exec (int       nthreads,
		float**   feature,    /* in: [npoints][nfeatures] */
		int       nfeatures,
		int       npoints,
		int       nclusters,
		float     threshold,
		int*      membership,
		random_t* randomPtr) /* out: [npoints] */
{
	int i;
	int j;
	int loop = 0;
	float delta;
	float** clusters;      /* out: [nclusters][nfeatures] */
#if GTM
	int** new_centers_len; /* [nclusters]: no. of points in each cluster */
	float** new_centers;   /* [nclusters][nfeatures] */
#else
	SingleElem<int>** new_centers_len;
	SingleElem<float>** new_centers;
#endif
	void* alloc_memory = NULL;
	args_t args;
	TIMER_T start;
	TIMER_T stop;

	/* Allocate space for returning variable clusters[] */
	clusters = (float**)malloc(nclusters * sizeof(float*));
	assert(clusters);
	clusters[0] = (float*)malloc(nclusters * nfeatures * sizeof(float));
	assert(clusters[0]);
	for (i = 1; i < nclusters; i++) {
		clusters[i] = clusters[i-1] + nfeatures;
	}

	/* Randomly pick cluster centers */
	for (i = 0; i < nclusters; i++) {
		int n = (int)(random_generate(randomPtr) % npoints);
		for (j = 0; j < nfeatures; j++) {
			clusters[i][j] = feature[n][j];
		}
	}

	for (i = 0; i < npoints; i++) {
		membership[i] = -1;
	}

	/*
	 * Need to initialize new_centers_len and new_centers[0] to all 0.
	 * Allocate clusters on different cache lines to reduce false sharing.
	 */
	{
#if GTM
		int cluster_size = sizeof(int) + sizeof(float) * nfeatures;
#else
		int cluster_size = sizeof(SingleElem<int>) + sizeof(SingleElem<float>) * nfeatures;
#endif
		const int cacheLineSize = 32;
		cluster_size += (cacheLineSize-1) - ((cluster_size-1) % cacheLineSize);
		alloc_memory = calloc(nclusters, cluster_size);
#if GTM
		new_centers_len = (int**) malloc(nclusters * sizeof(int*));
		new_centers = (float**) malloc(nclusters * sizeof(float*));
#else
		new_centers_len = (SingleElem<int>**) malloc(nclusters * sizeof(SingleElem<int> *));
		new_centers = (SingleElem<float> **) malloc(nclusters *sizeof(SingleElem<float> *));
#endif
		assert(alloc_memory && new_centers && new_centers_len);
		for (i = 0; i < nclusters; i++) {
#if GTM
			new_centers_len[i] = (int*)((char*)alloc_memory + cluster_size * i);
			new_centers[i] = (float*)((char*)alloc_memory + cluster_size * i + sizeof(int));
#else
			new_centers_len[i] = (SingleElem<int>*)((char*)alloc_memory + cluster_size * i);
			new_centers[i] = (SingleElem<float>*)((char*)alloc_memory + cluster_size * i + sizeof(SingleElem<int>));
			printf("cluster %d %d\n", i, new_centers_len[i]->read());
			int j;
			for(j=0; j < nfeatures; j++){
				printf("cluster %d %f\n", i, new_centers[i][j].read());
			}
#endif
		}
	}

	TIMER_READ(start);

	GOTO_SIM();

	do {
		delta = 0.0;

		args.feature         = feature;
		args.nfeatures       = nfeatures;
		args.npoints         = npoints;
		args.nclusters       = nclusters;
		args.membership      = membership;
		args.clusters        = clusters;
		args.new_centers_len = new_centers_len;
		args.new_centers     = new_centers;

#if GTM
		global_i = nthreads * CHUNK;
		global_delta = delta;
#else
		global_i.write(nthreads * CHUNK);
		global_delta.write(delta);
#endif

#if defined(OTM)
#pragma omp parallel
		{
			work(&args);
		}
#else
		printf("thread start!\n");
		thread_start(work, &args);
#endif
	 printf("thread end\n");
#if GTM
		delta = global_delta;
#else
		delta = global_delta.read();
#endif
		/* Replace old cluster centers with new_centers */
		for (i = 0; i < nclusters; i++) {
			for (j = 0; j < nfeatures; j++) {
				if (new_centers_len[i] > 0) {
#if GTM
					clusters[i][j] = new_centers[i][j] / *new_centers_len[i];
#else
					clusters[i][j] = new_centers[i][j].read() / new_centers_len[i]->read();
#endif
				}
#if GTM
				new_centers[i][j] = 0.0;   /* set back to 0 */
#else
				new_centers[i][j].write(0.0);   /* set back to 0 */
#endif
			}
#if GTM
			*new_centers_len[i] = 0;   /* set back to 0 */
#else
			new_centers_len[i]->write(0);
#endif
		}

		delta /= npoints;

	} while ((delta > threshold) && (loop++ < 500));

	GOTO_REAL();

	TIMER_READ(stop);
	global_time += TIMER_DIFF_SECONDS(start, stop);

	free(alloc_memory);
	free(new_centers);
	free(new_centers_len);

	return clusters;
}


/* =============================================================================
 *
 * End of normal.c
 *
 * =============================================================================
 */
