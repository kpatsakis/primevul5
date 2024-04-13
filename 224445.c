static SQLITE_NOINLINE int growOp3(Vdbe *p, int op, int p1, int p2, int p3){
  assert( p->nOpAlloc<=p->nOp );
  if( growOpArray(p, 1) ) return 1;
  assert( p->nOpAlloc>p->nOp );
  return sqlite3VdbeAddOp3(p, op, p1, p2, p3);
}