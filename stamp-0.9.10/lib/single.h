#include "sto/Transaction.hh"
#include "sto/TBox.hh"
template<class T> using Single = TBox<T>;
#define TM_SINGLE_TRANS_READ(var) (var.read())
#define TM_SINGLE_TRANS_READ_F(var) (var.read())
#define TM_SINGLE_TRANS_WRITE(var, val) (var.write(val))
#define TM_SINGLE_TRANS_WRITE_F(var, val) (var.write(val))
#define TM_SINGLE_SIMPLE_READ(var) (var.nontrans_read())
#define TM_SINGLE_SIMPLE_WRITE(var, val) (var.nontrans_write(val))
