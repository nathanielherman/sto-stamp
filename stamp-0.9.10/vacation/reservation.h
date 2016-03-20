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

#if defined(STO) || defined(BOOSTING)
#define reservation2
#endif
#include "tm.h"
#include "types.h"
#ifdef reservation2
#include "sto/TBox.hh"
#include "sto/TIntRange.hh"
#endif

typedef enum reservation_type {
    RESERVATION_CAR,
    RESERVATION_FLIGHT,
    RESERVATION_ROOM,
    NUM_RESERVATION_TYPE
} reservation_type_t;

typedef struct reservation_info {
    reservation_type_t type; long id; int price; /* holds price at time reservation was made */
} reservation_info_t;

#if defined(reservation2) 
//&& defined(STO)
typedef struct reservation {
    long id;
    int numUsed;
    int numFree;
    int numTotal;
    int price;
} _reservation_t;
#else
typedef struct reservation {
    long id;
    long numUsed;
    long numFree;
    long numTotal;
    long price;
} _reservation_t;
#endif

#ifdef reservation2
std::ostream& operator<<(std::ostream& w, const _reservation_t& r);
#endif

#if defined(reservation2) && defined(VACATION_PREDICATES)
class reservation_t: public TObject {
 public:
    typedef int count_type;
    static constexpr int total_key = 0;
    static constexpr int free_key = 1;
    static constexpr int used_key = 2;
    static constexpr int price_key = 3;
    struct rdata {
        count_type v[3];
    };
 private:
    struct rpred {
        TIntRange<count_type> r[3];
        rdata v, d;
        rpred(const rdata& rd)
            : v(rd) {
            for (int i = 0; i != 3; ++i)
                r[i] = TIntRange<count_type>::unconstrained();
            d.v[0] = d.v[1] = d.v[2] = 0;
        }
        friend std::ostream& operator<<(std::ostream& w, const rpred& rp) {
            w << '(';
            for (int i = 0; i < 3; ++i) {
                w << (i == total_key ? "total" : (i == free_key ? ", free" : ", used"))
                  << rp.r[i] << "@" << rp.v.v[i];
                if (rp.d.v[i])
                    w << "Δ" << rp.d.v[i];
            }
            return w << ')';
        }
    };
public:
    typedef _reservation_t T;
    typedef TWrapped<_reservation_t> WT;
    typedef WT::version_type version_type;
    typedef TIntRange<count_type> pred_type;
    typedef TIntRangeProxy<count_type> count_proxy;

    reservation_t(_reservation_t* rptr) {
        d_.access().v[used_key] = rptr->numUsed;
        d_.access().v[free_key] = rptr->numFree;
        d_.access().v[total_key] = rptr->numTotal;
        price_.access() = rptr->price;
    }

    T nontrans_read() const {
        return T{ 0, d_.access().v[used_key], d_.access().v[free_key], d_.access().v[total_key], price_.access() };
    }

    /* transaciton methods */
    count_proxy total() const {
        return make_proxy(my_rpred(), total_key);
    }

    count_proxy used() const {
        return make_proxy(my_rpred(), used_key);
    }

    count_proxy free() const {
        return make_proxy(my_rpred(), free_key);
    }

    count_type price() const {
        TransProxy item = Sto::item(this, price_key);
        return item.has_write() ? item.write_value<count_type>() : price_.read(item, price_vers_);
    }

    bool_t reservation_addToTotal(TM_ARGDECL int num) {
        auto it = my_item();
        auto& rp = my_rpred(it);
        if (make_proxy(rp, free_key) < -num)
            return FALSE;
        rp.d.v[total_key] += num;
        rp.d.v[free_key] += num;
        it.add_write();
        return TRUE;
    }

    bool_t reservation_addToTotal_seq(int num){
        if (d_.access().v[free_key] < -num)
            return FALSE;
        d_.access().v[total_key] += num;
        d_.access().v[free_key] += num;
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_make(TM_ARGDECL_ALONE){
        auto it = my_item();
        auto& rp = my_rpred(it);
        if (make_proxy(rp, free_key) < 1)
            return FALSE;
        rp.d.v[free_key] -= 1;
        rp.d.v[used_key] += 1;
        it.add_write();
        return TRUE;
    }

    bool_t reservation_make_seq(){
        if (d_.access().v[free_key] < 1)
            return FALSE;
        d_.access().v[free_key] -= 1;
        d_.access().v[used_key] += 1;
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_cancel (TM_ARGDECL_ALONE){
        auto it = my_item();
        auto& rp = my_rpred(it);
        if (make_proxy(rp, used_key) < 1)
            return FALSE;
        rp.d.v[free_key] += 1;
        rp.d.v[used_key] -= 1;
        it.add_write();
        return TRUE;
    }

    bool_t reservation_cancel_seq (){
        if (d_.access().v[free_key] >= d_.access().v[total_key])
            return FALSE;
        d_.access().v[free_key] -= 1;
        d_.access().v[used_key] += 1;
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_update_price (TM_ARGDECL int newPrice){
        if (newPrice < 0)
            return FALSE;
        Sto::item(this, price_key).add_write(newPrice);
        return TRUE;
    }

    bool_t reservation_update_price_seq (int newPrice){
        if (newPrice < 0)
            return FALSE;
        price_.access() = newPrice;
        checkReservation_seq();
        return TRUE;
    }


    bool lock(TransItem& item, Transaction& txn) {
        if (item.key<int>() == price_key)
            return txn.try_lock(item, price_vers_);
        else
            return txn.try_lock(item, d_vers_);
    }

    void unlock(TransItem& item) {
        if (item.key<int>() == price_key)
            price_vers_.unlock();
        else {
            checkReservation_seq();
            d_vers_.unlock();
        }
    }

    bool check_predicate(TransItem& item, Transaction& txn, bool committing) {
        TransProxy p(txn, item);
        rpred rp = item.predicate_value<rpred>();
        rdata rd = d_.wait_snapshot(p, d_vers_, committing);
        if (rp.r[0].verify(rd.v[0]) && rp.r[1].verify(rd.v[1]) && rp.r[2].verify(rd.v[2])) {
            if (p.has_write())
                p.clear_write().add_write(rp.d);
            return true;
        } else
            return false;
     }

    bool check(const TransItem& item, const Transaction&) {
        if (item.key<int>() == price_key)
            return item.check_version(price_vers_);
        else
            return item.check_version(d_vers_);
    }

    void install(TransItem& item, const Transaction& txn) {
        int key = item.key<int>();
        if (key == price_key) {
            price_.write(item.write_value<count_type>());
            price_vers_.set_version(txn.commit_tid());
        } else {
            rdata& wval = item.write_value<rdata>();
            d_.access().v[0] += wval.v[0];
            d_.access().v[1] += wval.v[1];
            d_.access().v[2] += wval.v[2];
            d_vers_.set_version(txn.commit_tid());
        }
    }

    void print(std::ostream& w, const TransItem& item) const {
        w << "{Reservation " << (void*) this;
        int key = item.key<int>();
        if (key == price_key) {
            w << ".price";
            if (item.has_read())
                w << " r" << item.read_value<version_type>();
            if (item.has_write())
                w << " =" << item.write_value<count_type>();
        } else {
            if (item.has_read())
                w << " r" << item.read_value<version_type>();
            else if (item.has_predicate())
                w << " P" << item.predicate_value<rpred>();
            if (item.has_write()) {
                auto& p = item.write_value<rdata>();
                for (int i = 0; i != 3; ++i)
                    if (p.v[i])
                        w << (i == total_key ? " totalΔ" : (i == free_key ? " freeΔ" : " usedΔ"))
                          << p.v[i];
            }
        }
        w << "}";
    }

private:
    TWrapped<rdata> d_;
    version_type d_vers_;
    TWrapped<count_type> price_;
    version_type price_vers_;

    TransProxy my_item() const {
        auto item = Sto::item(this, 0);
        if (!item.has_predicate()) {
            rdata c = d_.wait_snapshot(item, d_vers_, false);
            item.set_predicate(rpred(c));
        }
        return item;
    }
    static rpred& my_rpred(TransProxy item) {
        return item.predicate_value<rpred>();
    }
    rpred& my_rpred() const {
        return my_rpred(my_item());
    }
    static count_proxy make_proxy(rpred& rp, int key) {
        return count_proxy(&rp.r[key], rp.v.v[key], rp.d.v[key]);
    }

    inline void checkReservation_seq(){
        assert(d_.access().v[free_key] <= d_.access().v[total_key]);
        assert(d_.access().v[free_key] >= 0);
        assert(d_.access().v[total_key] >= 0);
        assert(d_.access().v[price_key] >= 0);
    }
};
#elif defined(reservation2)
class reservation_t 
#if defined(BOOSTING) && defined(STO)
: public TransUndoable
#endif
{
public:
    reservation_t(_reservation_t* _reservationPtr)
        : box_(*_reservationPtr) 
#ifdef BOOSTING
    , reslock_()
#endif
{
    }

    _reservation_t nontrans_read() const {
        return box_.nontrans_read();
    }

#ifdef BOOSTING
#define STRUCT_OFFSET(start, field) (&(field) - &(start))
#define RES_UNDO(_res, field) ADD_UNDO(reservation_t::_undoField, this, (void*)STRUCT_OFFSET(_res.numUsed, field), (void*)(uintptr_t)(field))
    // XXX: because the fields can all be ints (32 bits), we could cheat and store all of the data in two words
    // and then only need one undo per change (seems unlikely to make a big difference though)
    static void _undoField(void *self, void *c1, void *c2) {
      uintptr_t offset = (uintptr_t)c1;
      assert(offset < 4);
      int field = (int)(uintptr_t)c2;
      auto *me =  (reservation_t*)self;
      assert(me->reslock_.isWriteLocked());
      _reservation_t &res = me->box_.nontrans_access();
      (&res.numUsed)[offset] = field;
    }
#endif

    int total() const {
#ifndef BOOSTING
        _reservation_t r = box_;
#else
	TRANS_READ_LOCK((RWLock*)&reslock_);
	_reservation_t r = box_.nontrans_read();
#endif
        return r.numTotal;
    }

    int free() const {
#ifndef BOOSTING
        _reservation_t r = box_;
#else
	TRANS_READ_LOCK((RWLock*)&reslock_);
	_reservation_t r = box_.nontrans_read();
#endif
        return r.numFree;
    }

    int used() const {
#ifndef BOOSTING
        _reservation_t r = box_;
#else
        TRANS_READ_LOCK((RWLock*)&reslock_);
        _reservation_t r = box_.nontrans_read();
#endif
        return r.numUsed;
    }

    int price() const {
#ifndef BOOSTING
        _reservation_t r = box_;
#else
	TRANS_READ_LOCK((RWLock*)&reslock_);
	_reservation_t r = box_.nontrans_read();
#endif
        return r.price;
    }

    bool_t reservation_addToTotal(TM_ARGDECL int num){
#ifndef BOOSTING
        _reservation_t _reservation = box_;
        if (_reservation.numFree < -num){
            return FALSE;
        }

        _reservation.numTotal += num;
        _reservation.numFree += num;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
#else
	TRANS_WRITE_LOCK(&reslock_);
	_reservation_t& _reservation = box_.nontrans_access();
        if (_reservation.numFree < -num) {
            return FALSE;
        }
	RES_UNDO(_reservation, _reservation.numTotal);
	RES_UNDO(_reservation, _reservation.numFree);

        _reservation.numTotal += num;
        _reservation.numFree += num;
        checkReservation_seq();
        return TRUE;
#endif
    }

    bool_t reservation_addToTotal_seq(int num){
        _reservation_t _reservation = box_.nontrans_read();
        if (_reservation.numFree < -num){
            return FALSE;
        }

        _reservation.numTotal += num;
        _reservation.numFree += num;
        box_.nontrans_write(_reservation);
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_make(TM_ARGDECL_ALONE){
#ifndef BOOSTING
        _reservation_t _reservation = box_;
        if (_reservation.numFree < 1) {
            return FALSE;
        }

        _reservation.numFree--;
        _reservation.numUsed++;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
#else
	TRANS_WRITE_LOCK(&reslock_);
	_reservation_t& _reservation = box_.nontrans_access();
        if (_reservation.numFree < 1) {
            return FALSE;
        }
	RES_UNDO(_reservation, _reservation.numFree);
	RES_UNDO(_reservation, _reservation.numUsed);

        _reservation.numFree--;
        _reservation.numUsed++;
        checkReservation_seq();
        return TRUE;
#endif
    }

    bool_t reservation_make_seq(){
        _reservation_t _reservation = box_.nontrans_read();
        if (_reservation.numFree < 1) {
            return FALSE;
        }

        _reservation.numFree--;
        _reservation.numUsed++;
        box_.nontrans_write(_reservation);
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_cancel (TM_ARGDECL_ALONE){
#ifndef BOOSTING
        _reservation_t _reservation = box_;
        if (_reservation.numUsed < 1) {
            return FALSE;
        }

        _reservation.numFree++;
        _reservation.numUsed--;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
#else
	// assuming the cancel will most likely happen
	TRANS_WRITE_LOCK(&reslock_);
	_reservation_t& _reservation = box_.nontrans_access();
	if (_reservation.numUsed < 1) {
	  return FALSE;
	}
	RES_UNDO(_reservation, _reservation.numFree);
	RES_UNDO(_reservation, _reservation.numUsed);
	_reservation.numFree++;
	_reservation.numUsed--;
	checkReservation_seq();
        return TRUE;
#endif
    }

    bool_t reservation_cancel_seq (){
        _reservation_t _reservation = box_.nontrans_read();
        if (_reservation.numUsed < 1) {
            return FALSE;
        }

        _reservation.numFree++;
        _reservation.numUsed--;
        box_.nontrans_write(_reservation);
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_update_price (TM_ARGDECL int newPrice){
        if (newPrice < 0) {
            return FALSE;
        }

#ifndef BOOSTING
        _reservation_t _reservation = box_;
        _reservation.price = newPrice;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
#else
	TRANS_WRITE_LOCK(&reslock_);
	_reservation_t& _reservation = box_.nontrans_access();
	RES_UNDO(_reservation, _reservation.price);
	_reservation.price = newPrice;
	checkReservation_seq();
        return TRUE;
#endif
    }

    bool_t reservation_update_price_seq (int newPrice){
        if (newPrice < 0) {
            return FALSE;
        }

        _reservation_t _reservation = box_.nontrans_read();
        _reservation.price = newPrice;
        box_.nontrans_write(_reservation);
        checkReservation_seq();
        return TRUE;
    }

private:
    TBox<_reservation_t> box_;

#ifdef BOOSTING
    RWLock reslock_;
#endif

    inline void checkReservation (TM_ARGDECL_ALONE){
        _reservation_t _reservation = box_;

	// XXX: this is basically just a redundant opacity check.
        if(_reservation.numUsed < 0 ||
           _reservation.numFree < 0 ||
           _reservation.numTotal < 0 ||
           _reservation.numUsed + _reservation.numFree != _reservation.numTotal ||
           _reservation.price < 0){
            TM_RESTART();
        }
    }

    inline void checkReservation_seq(){
        _reservation_t _reservation = box_.nontrans_read();

        assert(_reservation.numUsed >= 0);
        assert(_reservation.numFree >= 0);
        assert(_reservation.numTotal >= 0);
        assert((_reservation.numUsed + _reservation.numFree) ==
               (_reservation.numTotal));
        assert(_reservation.price >= 0);
    }

};
#endif

#ifdef reservation2
/* alloc and free */
#if defined(STO)
#define SEQ_RESERVATION_ALLOC(_reservationPtr) (new reservation_t(_reservationPtr))
#define TM_RESERVATION_ALLOC(_reservationPtr) __talloc.transNew<reservation_t>(_reservationPtr)
#define TM_RESERVATION_FREE(reservationPtr) __talloc.transDelete(reservationPtr)
#else
#define SEQ_RESERVATION_ALLOC(_reservationPtr) ({ auto *ret = (reservation_t*)malloc(sizeof(reservation_t)); new (ret) reservation_t(_reservationPtr); ret; })
// destructor isn't trivial but we'll just leak it because mehh
#define TM_RESERVATION_ALLOC(_reservationPtr) ({ auto *ret = (reservation_t*)TM_MALLOC(sizeof(reservation_t)); new (ret) reservation_t(_reservationPtr); ret; })
#define TM_RESERVATION_FREE(reservationPtr) TM_FREE(reservationPtr)
#endif

#define TM_RESERVATION_SHARED_READ_TOTAL(reservationPtr) \
    reservationPtr->total()
#define TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) \
    reservationPtr->nontrans_read().numTotal
#define TM_RESERVATION_SHARED_READ_USED(reservationPtr) \
    reservationPtr->used()
#define TM_RESERVATION_SEQ_READ_USED(reservationPtr) \
    reservationPtr->unsafe_read().numUsed
#define TM_RESERVATION_SHARED_READ_FREE(reservationPtr) \
    reservationPtr->free()
#define TM_RESERVATION_SHARED_READ_PRICE(reservationPtr) \
    reservationPtr->price()

#else
typedef _reservation_t reservation_t;
#define SEQ_RESERVATION_ALLOC(_reservationPtr) _reservationPtr
#define TM_RESERVATION_ALLOC(_reservationPtr) _reservationPtr
#define TM_RESERVATION_FREE(reservationPtr) TM_FREE(reservationPtr)

#define TM_RESERVATION_SHARED_READ_TOTAL(reservationPtr) \
    ((long) TM_SHARED_READ(reservationPtr->numTotal))
#define TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) \
		reservationPtr->numTotal
#define TM_RESERVATION_SHARED_READ_USED(reservationPtr) \
    ((long) TM_SHARED_READ(reservationPtr->numUsed))
#define TM_RESERVATION_SEQ_READ_USED(reservationPtr) \
		reservationPtr->numUsed
#define TM_RESERVATION_SHARED_READ_FREE(reservationPtr) \
    ((long) TM_SHARED_READ(reservationPtr->numFree))
#define TM_RESERVATION_SEQ_READ_FREE(reservationPtr) \
		reservationPtr->numFree
#define TM_RESERVATION_SHARED_READ_PRICE(reservationPtr) \
    ((long) TM_SHARED_READ(reservationPtr->price))
#endif
/* =============================================================================
 * reservation_info_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_info_t*
reservation_info_alloc (TM_ARGDECL  reservation_type_t type, long id, int price);


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
int
reservation_info_compare (reservation_info_t* aPtr, reservation_info_t* bPtr);


/* =============================================================================
 * reservation_alloc
 * -- Returns NULL on failure
 * =============================================================================
 */
reservation_t*
reservation_alloc (TM_ARGDECL  long id, int price, int numTotal);

reservation_t*
reservation_alloc_seq (long id, int price, int numTotal);


/* =============================================================================
 * reservation_addToTotal
 * -- Adds if 'num' > 0, removes if 'num' < 0;
 * -- Returns TRUE on success, else FALSE
 * =============================================================================
 */
TM_CALLABLE
bool_t
reservation_addToTotal (TM_ARGDECL  reservation_t* reservationPtr, int num);

bool_t
reservation_addToTotal_seq (reservation_t* reservationPtr, int num);


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
reservation_updatePrice (TM_ARGDECL  reservation_t* reservationPtr, int newPrice);

bool_t
reservation_updatePrice_seq (reservation_t* reservationPtr, int newPrice);


/* =============================================================================
 * reservation_compare
 * -- Returns -1 if A < B, 0 if A = B, 1 if A > B
 * =============================================================================
 */
int
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
