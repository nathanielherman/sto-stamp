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

#ifdef reservation2
typedef struct reservation {
    long id;
    int numUsed;
    int numFree;
    int numTotal;
    int price;
} _reservation_t;

std::ostream& operator<<(std::ostream& w, const _reservation_t& r);
#else
typedef struct reservation {
    long id;
    long numUsed;
    long numFree;
    long numTotal;
    long price;
} _reservation_t;
#endif

#if defined(reservation2) && defined(VACATION_PREDICATES)
class reservation_t: public TObject {
    static constexpr int total_key = 0;
    static constexpr int free_key = 1;
    static constexpr int used_key = 2;
    static constexpr int price_key = 3;
public:
    typedef _reservation_t T;
    typedef TWrapped<_reservation_t> WT;
    typedef WT::version_type version_type;
    typedef int count_type;
    typedef TIntRange<count_type> pred_type;
    typedef TIntRangeProxy<count_type> count_proxy;

    reservation_t(_reservation_t* rptr) {
        c_[used_key].access() = rptr->numUsed;
        c_[free_key].access() = rptr->numFree;
        c_[total_key].access() = rptr->numTotal;
        c_[price_key].access() = rptr->price;
    }

    T nontrans_read() const {
        return T{ 0, c_[used_key].access(), c_[free_key].access(), c_[total_key].access(), c_[price_key].access() };
    }

    /* transaciton methods */
    count_proxy total() const {
        TransProxy item = my_item(total_key);
        return count_proxy(&ip(item), iorig(item), idelta(item));
    }

    count_proxy used() const {
        TransProxy item = my_item(used_key);
        return count_proxy(&ip(item), iorig(item), idelta(item));
        //TransProxy titem = my_item(total_key), fitem = my_item(free_key);
        //return count_proxy(&ip(titem), iorig(titem), idelta(titem) - iorig(fitem) - idelta(fitem));
    }

    count_proxy free() const {
	TransProxy item = my_item(free_key);
	return count_proxy(&ip(item), iorig(item), idelta(item));
    }

    count_type price() const {
        TransProxy item = Sto::item(this, price_key);
        return item.has_write() ? item.write_value<count_type>() : c_[price_key].read(item, price_vers_);
    }

    bool_t reservation_addToTotal(TM_ARGDECL int num){
        if (free() < -num)
            return FALSE;
        update(total_key, num);
        update(free_key, num);
        return TRUE;
    }

    bool_t reservation_addToTotal_seq(int num){
        if (c_[free_key].access() < -num)
            return FALSE;
        c_[total_key].access() += num;
        c_[free_key].access() += num;
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_make(TM_ARGDECL_ALONE){
        if (free() < 1)
            return FALSE;
        update(free_key, -1);
        update(used_key, 1);
        return TRUE;
    }

    bool_t reservation_make_seq(){
        if (c_[free_key].access() < 1)
            return FALSE;
        c_[free_key].access() -= 1;
        c_[used_key].access() += 1;
        checkReservation_seq();
        return TRUE;
    }

    bool_t reservation_cancel (TM_ARGDECL_ALONE){
        if (used() < 1)
            return FALSE;
        update(free_key, 1);
        update(used_key, -1);
        return TRUE;
    }

    bool_t reservation_cancel_seq (){
        if (c_[free_key].access() >= c_[total_key].access())
            return FALSE;
        c_[free_key].access() += 1;
        c_[used_key].access() -= 1;
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
        c_[price_key].access() = newPrice;
        checkReservation_seq();
        return TRUE;
    }


    bool lock(TransItem& item, Transaction&) {
        if (item.key<int>() == price_key)
            price_vers_.lock();
        else if (!vers_.is_locked_here())
            vers_.lock();
        return true;
    }

    void unlock(TransItem& item) {
        if (item.key<int>() == price_key)
            price_vers_.unlock();
        else if (vers_.is_locked_here()) {
            checkReservation_seq();
            vers_.unlock();
        }
    }

    bool check_predicate(TransItem& item, Transaction& txn, bool committing) {
        TransProxy p(txn, item);
        int key = item.key<int>();
        pred_type pred = item.predicate_value<pred_type>();
        count_type value = committing ? c_[key].read(p, vers_) : c_[key].snapshot(p, vers_);
        return pred.verify(value);
     }

    bool check(const TransItem& item, const Transaction&) {
        if (item.key<int>() == price_key)
            return item.check_version(price_vers_);
        else
            return item.check_version(vers_);
    }

    void install(TransItem& item, const Transaction& txn) {
        int key = item.key<int>();
        if (key == price_key) {
            c_[key].write(item.write_value<count_type>());
            price_vers_.set_version(txn.commit_tid());
        } else {
            pred_type wval = item.write_value<pred_type>();
            c_[key].write(c_[key].access() + wval.second - wval.first);
            vers_.set_version(txn.commit_tid());
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
            w << (key == total_key ? ".total" : (key == used_key ? ".used" : ".free"));
            if (item.has_read())
                w << " r" << item.read_value<version_type>();
            else if (item.has_predicate())
                w << " P" << item.predicate_value<pred_type>();
            if (item.has_write()) {
                auto& p = item.xwrite_value<pred_type>();
                w << " Δ" << (p.second - p.first);
            }
        }
        w << "}";
    }

private:
    TWrapped<count_type> c_[4];
    version_type vers_;
    version_type price_vers_;

    TransProxy my_item(int key) const {
        auto item = Sto::item(this, key);
        if (!item.has_predicate()) {
            item.set_predicate(pred_type::unconstrained());
            count_type c = c_[key].snapshot(item, vers_);
            item.template xwrite_value<pred_type>() = pred_type{c, c};
        }
        return item;
    }
    static pred_type& ip(TransProxy item) {
        return item.predicate_value<pred_type>();
    }
    static count_type& icur(TransProxy item) {
        return item.xwrite_value<pred_type>().second;
    }
    static count_type& iorig(TransProxy item) {
        return item.xwrite_value<pred_type>().first;
    }
    static count_type idelta(TransProxy item) {
        pred_type& info = item.xwrite_value<pred_type>();
        return info.second - info.first;
    }
    void update(int key, count_type delta) {
        TransProxy item = my_item(key);
        pred_type wval = item.xwrite_value<pred_type>();
        wval.second += delta;
        item.add_write(wval);
        ip(item).observe_ge(wval.first - wval.second); // can't go below 0
    }

    inline void checkReservation_seq(){
        assert(c_[free_key].access() <= c_[total_key].access());
        assert(c_[free_key].access() >= 0);
        assert(c_[total_key].access() >= 0);
        assert(c_[price_key].access() >= 0);
    }
};
#elif defined(reservation2)
class reservation_t {
public:
    reservation_t(_reservation_t* _reservationPtr)
        : box_(*_reservationPtr) {
    }

    _reservation_t nontrans_read() const {
        return box_.nontrans_read();
    }

    int total() const {
        _reservation_t r = box_;
        return r.numTotal;
    }

    int free() const {
        _reservation_t r = box_;
        return r.numFree;
    }

    int used() const {
        _reservation_t r = box_;
        return r.numUsed;
    }

    int price() const {
        _reservation_t r = box_;
        return r.price;
    }

    bool_t reservation_addToTotal(TM_ARGDECL int num){
        _reservation_t _reservation = box_;
        if (_reservation.numFree < -num){
            return FALSE;
        }

        _reservation.numTotal += num;
        _reservation.numFree += num;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
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
        _reservation_t _reservation = box_;
        if (_reservation.numFree < 1) {
            return FALSE;
        }

        _reservation.numFree--;
        _reservation.numUsed++;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
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
        _reservation_t _reservation = box_;
        if (_reservation.numUsed < 1) {
            return FALSE;
        }

        _reservation.numFree++;
        _reservation.numUsed--;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
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

        _reservation_t _reservation = box_;
        _reservation.price = newPrice;
        box_ = _reservation;
        checkReservation(TM_ARG_ALONE);
        return TRUE;
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

    inline void checkReservation (TM_ARGDECL_ALONE){
        _reservation_t _reservation = box_;

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
#define SEQ_RESERVATION_ALLOC(_reservationPtr) (new reservation_t(_reservationPtr))
#define TM_RESERVATION_ALLOC(_reservationPtr) SEQ_RESERVATION_ALLOC(_reservationPtr)
// XXX(nate) doesn't work yet: __talloc.transNew<reservation_t>(_reservationPtr)
#define TM_RESERVATION_FREE(reservationPtr) __talloc.transDelete(reservationPtr)

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
		TM_SHARED_READ(reservationPtr->numTotal)
#define TM_RESERVATION_SEQ_READ_TOTAL(reservationPtr) \
		reservationPtr->numTotal
#define TM_RESERVATION_SHARED_READ_USED(reservationPtr) \
		TM_SHARED_READ(reservationPtr->numUsed)
#define TM_RESERVATION_SEQ_READ_USED(reservationPtr) \
		reservationPtr->numUsed
#define TM_RESERVATION_SEQ_READ_FREE(reservationPtr) \
		reservationPtr->numFree
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
