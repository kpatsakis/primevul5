static void findConstInWhere(WhereConst *pConst, Expr *pExpr){
  Expr *pRight, *pLeft;
  if( pExpr==0 ) return;
  if( ExprHasProperty(pExpr, EP_FromJoin) ) return;
  if( pExpr->op==TK_AND ){
    findConstInWhere(pConst, pExpr->pRight);
    findConstInWhere(pConst, pExpr->pLeft);
    return;
  }
  if( pExpr->op!=TK_EQ ) return;
  pRight = pExpr->pRight;
  pLeft = pExpr->pLeft;
  assert( pRight!=0 );
  assert( pLeft!=0 );
  if( pRight->op==TK_COLUMN
   && !ExprHasProperty(pRight, EP_FixedCol)
   && sqlite3ExprIsConstant(pLeft)
   && sqlite3IsBinary(sqlite3ExprCompareCollSeq(pConst->pParse,pExpr))
  ){
    constInsert(pConst, pRight, pLeft);
  }else
  if( pLeft->op==TK_COLUMN
   && !ExprHasProperty(pLeft, EP_FixedCol)
   && sqlite3ExprIsConstant(pRight)
   && sqlite3IsBinary(sqlite3ExprCompareCollSeq(pConst->pParse,pExpr))
  ){
    constInsert(pConst, pLeft, pRight);
  }
}