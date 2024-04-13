void sqlite3VdbeChangeP5(Vdbe *p, u16 p5){
  assert( p->nOp>0 || p->db->mallocFailed );
  if( p->nOp>0 ) p->aOp[p->nOp-1].p5 = p5;
}