void sqlite3VdbeAddDblquoteStr(sqlite3 *db, Vdbe *p, const char *z){
  if( p ){
    int n = sqlite3Strlen30(z);
    DblquoteStr *pStr = sqlite3DbMallocRawNN(db,
                            sizeof(*pStr)+n+1-sizeof(pStr->z));
    if( pStr ){
      pStr->pNextStr = p->pDblStr;
      p->pDblStr = pStr;
      memcpy(pStr->z, z, n+1);
    }
  }
}