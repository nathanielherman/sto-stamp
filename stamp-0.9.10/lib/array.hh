#include "sto/Transaction.hh"
#include "sto/Array1.hh"

template<class T, int N> using Array = Array1<T, N>
#define TM_ARRAY_TRANS_READ(var, i) (var.transRead(__transaction, i))
#define TM_ARRAY_TRANS_WRITE(var, i, val) (var.transRead(__transaction, i))

