void sqlite3VdbeChangeP4(Vdbe *p, int addr, const char *zP4, int n){
  Op *pOp;
  sqlite3 *db;
  assert( p!=0 );
  db = p->db;
  assert( p->magic==VDBE_MAGIC_INIT );
  assert( p->aOp!=0 || db->mallocFailed );
  if( db->mallocFailed ){
    if( n!=P4_VTAB ) freeP4(db, n, (void*)*(char**)&zP4);
    return;
  }
  assert( p->nOp>0 );
  assert( addr<p->nOp );
  if( addr<0 ){
    addr = p->nOp - 1;
  }
  pOp = &p->aOp[addr];
  if( n>=0 || pOp->p4type ){
    vdbeChangeP4Full(p, pOp, zP4, n);
    return;
  }
  if( n==P4_INT32 ){
    /* Note: this cast is safe, because the origin data point was an int
    ** that was cast to a (const char *). */
    pOp->p4.i = SQLITE_PTR_TO_INT(zP4);
    pOp->p4type = P4_INT32;
  }else if( zP4!=0 ){
    assert( n<0 );
    pOp->p4.p = (void*)zP4;
    pOp->p4type = (signed char)n;
    if( n==P4_VTAB ) sqlite3VtabLock((VTable*)zP4);
  }
}