/* =============================================================================
 *
 * reservation.h
 * -- Representation of car, flight, and hotel relations
 *
 * =============================================================================
 *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
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


#ifndef RESERVATION_H
#define RESERVATION_H 1

#if defined(STO) && !defined(GTM)
#define reservation2
#endif
#include "tm.h"
#include "types.h"
#ifdef reservation2
#include "single.h"
#endif


typedef enum reservation_type {
    RESERVATION_CAR,
    RESERVATION_FLIGHT,
    RESERVATION_ROOM,
    NUM_RESERVATION_TYPE
} reservation_type_t;

typedef struct reservation_info {
    reservation_type_t type; long id; long price; /* holds price at time reservation was made */
} reservation_info_t;

typedef struct reservation {
    long id;
    long numUsed;
    long numFree;
    long numTotal;
    long price;
} _reservation_t;

#ifdef reservation2
class reservation_t: public SingleElem<_reservation_t*>{
		public:
				reservation_t(_reservation_t* _reservationPtr){
						write(_reservationPtr);
				}

				bool_t reservation_addToTotal(TM_ARGDECL long num){
						_reservation_t* _reservationPtr = transRead(TM_ARG_ALONE);
						if (_reservationPtr->numFree + num < 0){
								return FALSE;
						}

						_reservation_t* _newReservationPtr = copy_reservation(_reservationPtr);

						_newReservationPtr->numTotal += num;
						_newReservationPtr->numFree += num;

					  transWrite(TM_ARG _newReservationPtr);
						checkReservation(TM_ARG_ALONE);
						return TRUE;
				}

				bool_t reservation_make(TM_ARGDECL_ALONE){
						_reservation_t* _reservationPtr = transRead(TM_ARG_ALONE);
						long numFree = _reservationPtr->numFree;

						if (numFree < 1) {
								return FALSE;
						}

						_reservation_t* _newReservationPtr = copy_reservation(_reservationPtr);
						_newReservationPtr->numFree--;
						_newReservationPtr->numUsed++;

						transWrite(TM_ARG _newReservationPtr);
						checkReservation(TM_ARG_ALONE);
						return TRUE;
				}

				bool_t reservation_cancel (TM_ARGDECL_ALONE){
						_reservation_t* _reservationPtr = transRead(TM_ARG_ALONE);
						long numUsed = _reservationPtr->numUsed;

						if (numUsed < 1) {
								return FALSE;
						}

						_reservation_t* _newReservationPtr = copy_reservation(_reservationPtr);
						_newReservationPtr->numFree++;
						_newReservationPtr->numUsed--;

						transWrite(TM_ARG _newReservationPtr);
						checkReservation(TM_ARG_ALONE);
						return TRUE;
				}

				bool_t reservation_update_price (TM_ARGDECL double newPrice){
						if (newPrice < 0) {
								return FALSE;
						}
						
						_reservation_t* _reservationPtr = transRead(TM_ARG_ALONE);
						_reservation_t* _newReservationPtr = copy_reservation(_reservationPtr);
						_newReservationPtr->price = newPrice;

						transWrite(TM_ARG _newReservationPtr);
						checkReservation(TM_ARG_ALONE);
						return TRUE;
				}
				
				void install(TransItem& item) {
						memcpy(read(), item.template write_value<_reservation_t*>(), sizeof(_reservation_t));
						free(item.template write_value<_reservation_t*>());
						Versioning::inc_version(s_.version());
				}
		
		private:
				_reservation_t* copy_reservation( _reservation_t * _reservationPtr){
						_reservation_t * _newReservationPtr = (_reservation_t *)malloc(sizeof(_reservation_t));
						memcpy((void *)_newReservationPtr, (void *)_reservationPtr, sizeof(_reservation_t));
						return _newReservationPtr;
				}

				inline void checkReservation (TM_ARGDECL_ALONE){
						_reservation_t* _reservationPtr = transRead(TM_ARG_ALONE);

						if(_reservationPtr->numUsed < 0 || 
										_reservationPtr->numFree < 0 ||
										_reservationPtr->numTotal < 0 ||
										_reservationPtr->numUsed + _reservationPtr->numFree != _reservationPtr->numTotal ||
										_reservationPtr->price < 0){
								TM_RESTART();
						}
				}
};

/* alloc and free */
#define TM_RESERVATION_ALLOC(_reservationPtr) new reservation_t(_reservationPtr)
#define TM_RESERVATION_FREE(reservationPtr) TM_FREE(reservationPtr) 

#define TM_RESERVATION_SHARED_READ_TOTAL(reservationPtr) \
		reservationPtr->transRead(TM_ARG_ALONE)->numTotal
#define TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) \
		reservationPtr->read()->numTotal
#define TM_RESERVATION_SHARED_READ_USED(reservationPtr) \
		reservationPtr->transRead(TM_ARG_ALONE)->numUsed
#define TM_RESERVATION_SEQ_READ_USED(reservationPtr) \
		reservationPtr->read()->numUsed
#define TM_RESERVATION_SHARED_READ_FREE(reservationPtr) \
		reservationPtr->transRead(TM_ARG_ALONE)->numFree
#define TM_RESERVATION_SHARED_READ_PRICE(reservationPtr) \
		reservationPtr->transRead(TM_ARG_ALONE)->price
#else
typedef _reservation_t reservation_t;
#define TM_RESERVATION_ALLOC(_reservationPtr) _reservationPtr
#define TM_RESERVATION_FREE(reservationPtr) TM_FREE(reservationPtr) 

#define TM_RESERVATION_SHARED_READ_TOTAL(reservationPtr) \
		TM_SHARED_READ(reservationPtr->numTotal)
#define TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) \
		reservationPtr->numTotal
#define TM_RESERVATION_SHARED_READ_USED(reservationPtr) \
		TM_SHARED_READ(reservationPtr->numUsed)
#define TM_RESERVATION_SEQ_READ_USED(reservationPtr) \
		reservationPtr->numUsed
#define TM_RESERVATION_SHARED_READ_FREE(reservationPtr) \
		TM_SHARED_READ(reservationPtr->numFree)
#define TM_RESERVATION_SHARED_READ_PRICE(reservationPtr) \
		TM_SHARED_READ(reservationPtr->price)
#endif

/* =============================================================================
 * reservation_info_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_info_t*
reservation_info_alloc (TM_ARGDECL  reservation_type_t type, long id, long price);


/* =============================================================================
 * reservation_info_free
 * =============================================================================
 */
void
reservation_info_free (TM_ARGDECL  reservation_info_t* reservationInfoPtr);


/* =============================================================================
 * reservation_info_compare
 * -- Returns -1 if A < B, 0 if A = B, 1 if A > B
 * =============================================================================
 */
long
reservation_info_compare (reservation_info_t* aPtr, reservation_info_t* bPtr);


/* =============================================================================
 * reservation_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_t*
reservation_alloc (TM_ARGDECL  long id, long price, long numTotal);

reservation_t*
reservation_alloc_seq (long id, long price, long numTotal);


/* =============================================================================
 * reservation_addToTotal
 * -- Adds if 'num' > 0, removes if 'num' < 0;
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
TM_CALLABLE
bool_t
reservation_addToTotal (TM_ARGDECL  reservation_t* reservationPtr, long num);

bool_t
reservation_addToTotal_seq (reservation_t* reservationPtr, long num);


/* =============================================================================
 * reservation_make
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
TM_CALLABLE
bool_t
reservation_make (TM_ARGDECL  reservation_t* reservationPtr);

bool_t
reservation_make_seq (reservation_t* reservationPtr);


/* =============================================================================
 * reservation_cancel
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
TM_CALLABLE
bool_t
reservation_cancel (TM_ARGDECL  reservation_t* reservationPtr);

bool_t
reservation_cancel_seq (reservation_t* reservationPtr);


/* =============================================================================
 * reservation_updatePrice
 * -- Failure if 'price' < 0
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
TM_CALLABLE
bool_t
reservation_updatePrice (TM_ARGDECL  reservation_t* reservationPtr, long newPrice);

bool_t
reservation_updatePrice_seq (reservation_t* reservationPtr, long newPrice);


/* =============================================================================
 * reservation_compare
 * -- Returns -1 if A < B, 0 if A = B, 1 if A > B
 * =============================================================================
 */
long
reservation_compare (_reservation_t* aPtr, _reservation_t* bPtr);


/* =============================================================================
 * reservation_hash
 * =============================================================================
 */
ulong_t
reservation_hash (_reservation_t* reservationPtr);


/* =============================================================================
 * reservation_free
 * =============================================================================
 */
void
reservation_free (TM_ARGDECL  reservation_t* reservationPtr);

#define RESERVATION_INFO_ALLOC(type, id, price) \
    reservation_info_alloc(TM_ARG  type, id, price)
#define RESERVATION_INFO_FREE(r) \
    reservation_info_free(TM_ARG  r)

#define RESERVATION_ALLOC(id, price, tot) \
    reservation_alloc(TM_ARG  id, price, tot)
#define RESERVATION_ADD_TO_TOTAL(r, num) \
    reservation_addToTotal(TM_ARG  r, num)
#define RESERVATION_MAKE(r) \
    reservation_make(TM_ARG  r)
#define RESERVATION_CANCEL(r) \
    reservation_cancel(TM_ARG  r)
#define RESERVATION_UPDATE_PRICE(r, price) \
    reservation_updatePrice(TM_ARG  r, price)
#define RESERVATION_FREE(r) \
    reservation_free(TM_ARG  r)

#endif /* RESERVATION_H */


/* =============================================================================
 *
 * End of reservation.h
 *
 * =============================================================================
 */
