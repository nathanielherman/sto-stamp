#include "sto/Transaction.hh"
#include "sto/SingleElem.hh"
template<class T> using Single = SingleElem<T>;
#define TM_SINGLE_TRANS_READ(var) (var.transRead(__transaction))
#define TM_SINGLE_TRANS_READ_F(var) (var.transRead(__transaction))
#define TM_SINGLE_TRANS_WRITE(var, val) (var.transWrite(__transaction, val))
#define TM_SINGLE_TRANS_WRITE_F(var, val) (var.transWrite(__transaction, val))
#define TM_SINGLE_SIMPLE_READ(var) (var.read())
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var.write(val))
