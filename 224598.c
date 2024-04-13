static void freeEphemeralFunction(sqlite3 *db, FuncDef *pDef){
  if( (pDef->funcFlags & SQLITE_FUNC_EPHEM)!=0 ){
    sqlite3DbFreeNN(db, pDef);
  }
}