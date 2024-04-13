static SQLITE_NOINLINE void freeP4Mem(sqlite3 *db, Mem *p){
  if( p->szMalloc ) sqlite3DbFree(db, p->zMalloc);
  sqlite3DbFreeNN(db, p);
}