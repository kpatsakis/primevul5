static void SQLITE_NOINLINE vdbeChangeP4Full(
  Vdbe *p,
  Op *pOp,
  const char *zP4,
  int n
){
  if( pOp->p4type ){
    freeP4(p->db, pOp->p4type, pOp->p4.p);
    pOp->p4type = 0;
    pOp->p4.p = 0;
  }
  if( n<0 ){
    sqlite3VdbeChangeP4(p, (int)(pOp - p->aOp), zP4, n);
  }else{
    if( n==0 ) n = sqlite3Strlen30(zP4);
    pOp->p4.z = sqlite3DbStrNDup(p->db, zP4, n);
    pOp->p4type = P4_DYNAMIC;
  }
}