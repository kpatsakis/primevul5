void sqlite3VdbeClearObject(sqlite3 *db, Vdbe *p){
  SubProgram *pSub, *pNext;
  assert( p->db==0 || p->db==db );
  releaseMemArray(p->aColName, p->nResColumn*COLNAME_N);
  for(pSub=p->pProgram; pSub; pSub=pNext){
    pNext = pSub->pNext;
    vdbeFreeOpArray(db, pSub->aOp, pSub->nOp);
    sqlite3DbFree(db, pSub);
  }
  if( p->magic!=VDBE_MAGIC_INIT ){
    releaseMemArray(p->aVar, p->nVar);
    sqlite3DbFree(db, p->pVList);
    sqlite3DbFree(db, p->pFree);
  }
  vdbeFreeOpArray(db, p->aOp, p->nOp);
  sqlite3DbFree(db, p->aColName);
  sqlite3DbFree(db, p->zSql);
#ifdef SQLITE_ENABLE_NORMALIZE
  sqlite3DbFree(db, p->zNormSql);
  {
    DblquoteStr *pThis, *pNext;
    for(pThis=p->pDblStr; pThis; pThis=pNext){
      pNext = pThis->pNextStr;
      sqlite3DbFree(db, pThis);
    }
  }
#endif
#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
  {
    int i;
    for(i=0; i<p->nScan; i++){
      sqlite3DbFree(db, p->aScan[i].zName);
    }
    sqlite3DbFree(db, p->aScan);
  }
#endif
}