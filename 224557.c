void sqlite3VdbeSetNumCols(Vdbe *p, int nResColumn){
  int n;
  sqlite3 *db = p->db;

  if( p->nResColumn ){
    releaseMemArray(p->aColName, p->nResColumn*COLNAME_N);
    sqlite3DbFree(db, p->aColName);
  }
  n = nResColumn*COLNAME_N;
  p->nResColumn = (u16)nResColumn;
  p->aColName = (Mem*)sqlite3DbMallocRawNN(db, sizeof(Mem)*n );
  if( p->aColName==0 ) return;
  initMemArray(p->aColName, n, db, MEM_Null);
}