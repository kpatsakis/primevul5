Vdbe *sqlite3VdbeCreate(Parse *pParse){
  sqlite3 *db = pParse->db;
  Vdbe *p;
  p = sqlite3DbMallocRawNN(db, sizeof(Vdbe) );
  if( p==0 ) return 0;
  memset(&p->aOp, 0, sizeof(Vdbe)-offsetof(Vdbe,aOp));
  p->db = db;
  if( db->pVdbe ){
    db->pVdbe->pPrev = p;
  }
  p->pNext = db->pVdbe;
  p->pPrev = 0;
  db->pVdbe = p;
  p->magic = VDBE_MAGIC_INIT;
  p->pParse = pParse;
  pParse->pVdbe = p;
  assert( pParse->aLabel==0 );
  assert( pParse->nLabel==0 );
  assert( p->nOpAlloc==0 );
  assert( pParse->szOpAlloc==0 );
  sqlite3VdbeAddOp2(p, OP_Init, 0, 1);
  return p;
}