void sqlite3VdbeVerifyNoMallocRequired(Vdbe *p, int N){
  assert( p->nOp + N <= p->nOpAlloc );
}