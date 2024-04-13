void sqlite3VdbeChangeP2(Vdbe *p, int addr, int val){
  sqlite3VdbeGetOp(p,addr)->p2 = val;
}