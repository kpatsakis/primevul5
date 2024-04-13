void sqlite3VdbeChangeP3(Vdbe *p, int addr, int val){
  sqlite3VdbeGetOp(p,addr)->p3 = val;
}