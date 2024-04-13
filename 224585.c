static SQLITE_NOINLINE void freeP4FuncCtx(sqlite3 *db, sqlite3_context *p){
  freeEphemeralFunction(db, p->pFunc);
  sqlite3DbFreeNN(db, p);
}