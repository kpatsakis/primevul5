static Expr *sqlite3WindowOffsetExpr(Parse *pParse, Expr *pExpr){
  if( 0==sqlite3ExprIsConstant(pExpr) ){
    if( IN_RENAME_OBJECT ) sqlite3RenameExprUnmap(pParse, pExpr);
    sqlite3ExprDelete(pParse->db, pExpr);
    pExpr = sqlite3ExprAlloc(pParse->db, TK_NULL, 0, 0);
  }
  return pExpr;
}