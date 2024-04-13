int sqlite3VdbeChangeToNoop(Vdbe *p, int addr){
  VdbeOp *pOp;
  if( p->db->mallocFailed ) return 0;
  assert( addr>=0 && addr<p->nOp );
  pOp = &p->aOp[addr];
  freeP4(p->db, pOp->p4type, pOp->p4.p);
  pOp->p4type = P4_NOTUSED;
  pOp->p4.z = 0;
  pOp->opcode = OP_Noop;
  return 1;
}