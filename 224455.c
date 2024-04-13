Window *sqlite3WindowDup(sqlite3 *db, Expr *pOwner, Window *p){
  Window *pNew = 0;
  if( ALWAYS(p) ){
    pNew = sqlite3DbMallocZero(db, sizeof(Window));
    if( pNew ){
      pNew->zName = sqlite3DbStrDup(db, p->zName);
      pNew->zBase = sqlite3DbStrDup(db, p->zBase);
      pNew->pFilter = sqlite3ExprDup(db, p->pFilter, 0);
      pNew->pFunc = p->pFunc;
      pNew->pPartition = sqlite3ExprListDup(db, p->pPartition, 0);
      pNew->pOrderBy = sqlite3ExprListDup(db, p->pOrderBy, 0);
      pNew->eFrmType = p->eFrmType;
      pNew->eEnd = p->eEnd;
      pNew->eStart = p->eStart;
      pNew->eExclude = p->eExclude;
      pNew->regResult = p->regResult;
      pNew->pStart = sqlite3ExprDup(db, p->pStart, 0);
      pNew->pEnd = sqlite3ExprDup(db, p->pEnd, 0);
      pNew->pOwner = pOwner;
      pNew->bImplicitFrame = p->bImplicitFrame;
    }
  }
  return pNew;
}