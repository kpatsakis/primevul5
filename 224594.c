void sqlite3VdbeVerifyNoResultRow(Vdbe *p){
  int i;
  for(i=0; i<p->nOp; i++){
    assert( p->aOp[i].opcode!=OP_ResultRow );
  }
}