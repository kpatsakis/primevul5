int sqlite3VdbeCloseStatement(Vdbe *p, int eOp){
  if( p->db->nStatement && p->iStatement ){
    return vdbeCloseStatement(p, eOp);
  }
  return SQLITE_OK;
}