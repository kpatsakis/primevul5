static void constInsert(
  WhereConst *pConst,  /* The WhereConst into which we are inserting */
  Expr *pColumn,       /* The COLUMN part of the constraint */
  Expr *pValue,        /* The VALUE part of the constraint */
  Expr *pExpr          /* Overall expression: COLUMN=VALUE or VALUE=COLUMN */
){
  int i;
  assert( pColumn->op==TK_COLUMN );
  assert( sqlite3ExprIsConstant(pValue) );

  if( !ExprHasProperty(pValue, EP_FixedCol) && sqlite3ExprAffinity(pValue)!=0 ){
    return;
  }
  if( !sqlite3IsBinary(sqlite3ExprCompareCollSeq(pConst->pParse,pExpr)) ){
    return;
  }

  /* 2018-10-25 ticket [cf5ed20f]
  ** Make sure the same pColumn is not inserted more than once */
  for(i=0; i<pConst->nConst; i++){
    const Expr *pE2 = pConst->apExpr[i*2];
    assert( pE2->op==TK_COLUMN );
    if( pE2->iTable==pColumn->iTable
     && pE2->iColumn==pColumn->iColumn
    ){
      return;  /* Already present.  Return without doing anything. */
    }
  }

  pConst->nConst++;
  pConst->apExpr = sqlite3DbReallocOrFree(pConst->pParse->db, pConst->apExpr,
                         pConst->nConst*2*sizeof(Expr*));
  if( pConst->apExpr==0 ){
    pConst->nConst = 0;
  }else{
    if( ExprHasProperty(pValue, EP_FixedCol) ){
      pValue = pValue->pLeft;
    }
    pConst->apExpr[pConst->nConst*2-2] = pColumn;
    pConst->apExpr[pConst->nConst*2-1] = pValue;
  }
}