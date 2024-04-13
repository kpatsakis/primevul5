void sqlite3WindowDelete(sqlite3 *db, Window *p){
  if( p ){
    sqlite3WindowUnlinkFromSelect(p);
    sqlite3ExprDelete(db, p->pFilter);
    sqlite3ExprListDelete(db, p->pPartition);
    sqlite3ExprListDelete(db, p->pOrderBy);
    sqlite3ExprDelete(db, p->pEnd);
    sqlite3ExprDelete(db, p->pStart);
    sqlite3DbFree(db, p->zName);
    sqlite3DbFree(db, p->zBase);
    sqlite3DbFree(db, p);
  }
}