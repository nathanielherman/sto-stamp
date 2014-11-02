/* =============================================================================
 *
 * reservation.c
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


#include <assert.h>
#include <stdlib.h>
#include "memory.h"
#include "reservation.h"
#include "tm.h"
#include "types.h"

/* =============================================================================
 * DECLARATION OF TM_CALLABLE FUNCTIONS
 * =============================================================================
 */

TM_CALLABLE
static void
checkReservation (TM_ARGDECL  reservation_t* reservationPtr);

/* =============================================================================
 * reservation_info_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_info_t*
reservation_info_alloc (TM_ARGDECL  reservation_type_t type, long id, long price)
{
    reservation_info_t* reservationInfoPtr;

    reservationInfoPtr = (reservation_info_t*)TM_MALLOC(sizeof(reservation_info_t));
    if (reservationInfoPtr != NULL) {
        reservationInfoPtr->type = type;
        reservationInfoPtr->id = id;
        reservationInfoPtr->price = price;
    }

    return reservationInfoPtr;
}


/* =============================================================================
 * reservation_info_free
 * =============================================================================
 */
void
reservation_info_free (TM_ARGDECL  reservation_info_t* reservationInfoPtr)
{
    TM_FREE(reservationInfoPtr);
}


/* =============================================================================
 * reservation_info_compare
 * -- Returns -1 if A < B, 0 if A = B, 1 if A > B
 * =============================================================================
 */
long
reservation_info_compare (reservation_info_t* aPtr, reservation_info_t* bPtr)
{
    long typeDiff;

    typeDiff = aPtr->type - bPtr->type;

    return ((typeDiff != 0) ? (typeDiff) : (aPtr->id - bPtr->id));
}


/* =============================================================================
 * checkReservation
 * -- Check if consistent
 * =============================================================================
 */
static void
checkReservation (TM_ARGDECL  reservation_t* reservationPtr)
{
	  _reservation_t _reservation = TM_RESERVATION_SHARED_READ(reservationPtr);

    if(_reservation.numUsed < 0 || 
						_reservation.numFree < 0 ||
						_reservation.numTotal < 0 ||
						_reservation.numUsed + _reservation.numFree != _reservation.numTotal ||
						_reservation.price < 0){
        TM_RESTART();
    }
}

#define CHECK_RESERVATION(reservation) \
    checkReservation(TM_ARG  reservation)

static void
checkReservation_seq (reservation_t* reservationPtr)
{
	  _reservation_t _reservation = TM_RESERVATION_SEQ_READ(reservationPtr);
    assert(_reservation.numUsed >= 0);
    assert(_reservation.numFree >= 0);
    assert(_reservation.numTotal >= 0);
    assert((_reservation.numUsed + _reservation.numFree) ==
           (_reservation.numTotal));
    assert(_reservation.price >= 0);
}


/* =============================================================================
 * reservation_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_t*
reservation_alloc (TM_ARGDECL  long id, long numTotal, long price)
{
    _reservation_t* _reservationPtr;

    _reservationPtr = (_reservation_t*)TM_MALLOC(sizeof(_reservation_t));
    if (_reservationPtr != NULL) {
        _reservationPtr->id = id;
        _reservationPtr->numUsed = 0;
        _reservationPtr->numFree = numTotal;
        _reservationPtr->numTotal = numTotal;
        _reservationPtr->price = price;
        CHECK_RESERVATION(_reservationPtr);
    }

    return TM_RESERVATION_ALLOC(_reservationPtr);
}


reservation_t*
reservation_alloc_seq (long id, long numTotal, long price)
{
    _reservation_t* _reservationPtr;

    _reservationPtr = (_reservation_t*)malloc(sizeof(_reservation_t));
    if (_reservationPtr != NULL) {
        _reservationPtr->id = id;
        _reservationPtr->numUsed = 0;
        _reservationPtr->numFree = numTotal;
        _reservationPtr->numTotal = numTotal;
        _reservationPtr->price = price;
        checkReservation_seq(_reservationPtr);
    }

    return TM_RESERVATION_ALLOC(_reservationPtr);
}


/* =============================================================================
 * reservation_addToTotal
 * -- Adds if 'num' > 0, removes if 'num' < 0;
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
bool_t
reservation_addToTotal (TM_ARGDECL  reservation_t* reservationPtr, long num)
{
    long numFree = TM_RESERVATION_SHARED_READ_FREE(reservationPtr);

    if (numFree + num < 0) {
        return FALSE;
    }
    
		long numTotal = TM_RESERVATION_SHARED_READ_TOTAL(reservationPtr);
		
		numFree += num;
		numTotal += num;
		TM_RESERVATION_SHARED_UPDATE_TOTAL_AND_FREE(reservationPtr, numFree, numTotal);
    CHECK_RESERVATION(reservationPtr);

    return TRUE;
}


bool_t
reservation_addToTotal_seq (reservation_t* reservationPtr, long num)
{
    long numFree = TM_RESERVATION_SEQ_READ_FREE(reservationPtr);
    if (numFree + num < 0) {
        return FALSE;
    }

		numFree += num;
		long numTotal = TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) + num;

		TM_RESERVATION_SEQ_UPDATE_TOTAL_AND_FREE(reservationPtr, numFree, numTotal);
    checkReservation_seq(reservationPtr);

    return TRUE;
}


/* =============================================================================
 * reservation_make
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
bool_t
reservation_make (TM_ARGDECL  reservation_t* reservationPtr)
{
    long numFree = (long)TM_RESERVATION_SHARED_READ_FREE(reservationPtr);
		long numUsed = (long)TM_RESERVATION_SHARED_READ_USED(reservationPtr);

    if (numFree < 1) {
        return FALSE;
    }

		numFree--;
		numUsed++;

		TM_RESERVATION_SHARED_UPDATE_USED_AND_FREE(reservationPtr, numUsed, numFree);
    CHECK_RESERVATION(reservationPtr);

    return TRUE;
}


bool_t
reservation_make_seq (reservation_t* reservationPtr)
{
    long numFree = (long)TM_RESERVATION_SEQ_READ_FREE(reservationPtr);
		long numUsed = (long)TM_RESERVATION_SEQ_READ_USED(reservationPtr);
    
		if (numFree < 1) {
        return FALSE;
    }

		numFree--;
		numUsed++;
		
		TM_RESERVATION_SEQ_UPDATE_USED_AND_FREE(reservationPtr, numUsed, numFree);
    checkReservation_seq(reservationPtr);

    return TRUE;
}


/* =============================================================================
 * reservation_cancel
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
bool_t
reservation_cancel (TM_ARGDECL  reservation_t* reservationPtr)
{
    long numFree = (long)TM_RESERVATION_SHARED_READ_FREE(reservationPtr);
		long numUsed = (long)TM_RESERVATION_SHARED_READ_USED(reservationPtr);

    if (numUsed < 1) {
        return FALSE;
    }

		numUsed--;
		numFree++;

		TM_RESERVATION_SHARED_UPDATE_USED_AND_FREE(reservationPtr, numUsed, numFree);
    CHECK_RESERVATION(reservationPtr);

    return TRUE;
}


bool_t
reservation_cancel_seq (reservation_t* reservationPtr)
{
    long numFree = (long)TM_RESERVATION_SEQ_READ_FREE(reservationPtr);
		long numUsed = (long)TM_RESERVATION_SEQ_READ_USED(reservationPtr);
    
		if (numUsed < 1) {
        return FALSE;
    }
		
		numUsed--;
		numFree++;

		TM_RESERVATION_SEQ_UPDATE_USED_AND_FREE(reservationPtr, numUsed, numFree);
    checkReservation_seq(reservationPtr);

    return TRUE;
}


/* =============================================================================
 * reservation_updatePrice
 * -- Failure if 'price' < 0
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
bool_t
reservation_updatePrice (TM_ARGDECL  reservation_t* reservationPtr, long newPrice)
{
    if (newPrice < 0) {
        return FALSE;
    }

    TM_RESERVATION_SHARED_UPDATE_PRICE(reservationPtr, newPrice);
    CHECK_RESERVATION(reservationPtr);

    return TRUE;
}


bool_t
reservation_updatePrice_seq (reservation_t* reservationPtr, long newPrice)
{
    if (newPrice < 0) {
        return FALSE;
    }

		TM_RESERVATION_SEQ_UPDATE_PRICE(reservationPtr, newPrice);
    checkReservation_seq(reservationPtr);

    return TRUE;
}


/* =============================================================================
 * reservation_compare
 * -- Returns -1 if A < B, 0 if A = B, 1 if A > B
 * =============================================================================
 */
long
reservation_compare (_reservation_t* aPtr, _reservation_t* bPtr)
{
    return (aPtr->id - bPtr->id);
}


/* =============================================================================
 * reservation_hash
 * =============================================================================
 */
ulong_t
reservation_hash (_reservation_t* reservationPtr)
{
    /* Separate tables for cars, flights, etc, so no need to use 'type' */
    return (ulong_t)reservationPtr->id;
}


/* =============================================================================
 * reservation_free
 * =============================================================================
 */
void
reservation_free (TM_ARGDECL  reservation_t* reservationPtr)
{
    TM_RESERVATION_FREE(reservationPtr);
}


/* =============================================================================
 * TEST_RESERVATION
 * =============================================================================
 */
#ifdef TEST_RESERVATION


#include <assert.h>
#include <stdio.h>


int
main ()
{
    reservation_info_t* reservationInfo1Ptr;
    reservation_info_t* reservationInfo2Ptr;
    reservation_info_t* reservationInfo3Ptr;

    reservation_t* reservation1Ptr;
    reservation_t* reservation2Ptr;
    reservation_t* reservation3Ptr;

    assert(memory_init(1, 4, 2));

    puts("Starting...");

    reservationInfo1Ptr = reservation_info_alloc(0, 0, 0);
    reservationInfo2Ptr = reservation_info_alloc(0, 0, 1);
    reservationInfo3Ptr = reservation_info_alloc(2, 0, 1);

    /* Test compare */
    assert(reservation_info_compare(reservationInfo1Ptr, reservationInfo2Ptr) == 0);
    assert(reservation_info_compare(reservationInfo1Ptr, reservationInfo3Ptr) > 0);
    assert(reservation_info_compare(reservationInfo2Ptr, reservationInfo3Ptr) > 0);

    reservation1Ptr = reservation_alloc(0, 0, 0);
    reservation2Ptr = reservation_alloc(0, 0, 1);
    reservation3Ptr = reservation_alloc(2, 0, 1);

    /* Test compare */
    assert(reservation_compare(reservation1Ptr, reservation2Ptr) == 0);
    assert(reservation_compare(reservation1Ptr, reservation3Ptr) != 0);
    assert(reservation_compare(reservation2Ptr, reservation3Ptr) != 0);

    /* Cannot reserve if total is 0 */
    assert(!reservation_make(reservation1Ptr));

    /* Cannot cancel if used is 0 */
    assert(!reservation_cancel(reservation1Ptr));

    /* Cannot update with negative price */
    assert(!reservation_updatePrice(reservation1Ptr, -1));

    /* Cannot make negative total */
    assert(!reservation_addToTotal(reservation1Ptr, -1));

    /* Update total and price */
    assert(reservation_addToTotal(reservation1Ptr, 1));
    assert(reservation_updatePrice(reservation1Ptr, 1));
    assert(reservation1Ptr->numUsed == 0);
    assert(reservation1Ptr->numFree == 1);
    assert(reservation1Ptr->numTotal == 1);
    assert(reservation1Ptr->price == 1);
    checkReservation(reservation1Ptr);

    /* Make and cancel reservation */
    assert(reservation_make(reservation1Ptr));
    assert(reservation_cancel(reservation1Ptr));
    assert(!reservation_cancel(reservation1Ptr));

    reservation_info_free(reservationInfo1Ptr);
    reservation_info_free(reservationInfo2Ptr);
    reservation_info_free(reservationInfo3Ptr);

    reservation_free(reservation1Ptr);
    reservation_free(reservation2Ptr);
    reservation_free(reservation3Ptr);

    puts("All tests passed.");

    return 0;
}


#endif /* TEST_RESERVATION */


/* =============================================================================
 *
 * End of reservation.c
 *
 * =============================================================================
 */
