void sqlite3VdbeDelete(Vdbe *p){
  sqlite3 *db;

  assert( p!=0 );
  db = p->db;
  assert( sqlite3_mutex_held(db->mutex) );
  sqlite3VdbeClearObject(db, p);
  if( p->pPrev ){
    p->pPrev->pNext = p->pNext;
  }else{
    assert( db->pVdbe==p );
    db->pVdbe = p->pNext;
  }
  if( p->pNext ){
    p->pNext->pPrev = p->pPrev;
  }
  p->magic = VDBE_MAGIC_DEAD;
  p->db = 0;
  sqlite3DbFreeNN(db, p);
}