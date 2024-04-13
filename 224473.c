static int windowExprGtZero(Parse *pParse, Expr *pExpr){
  int ret = 0;
  sqlite3 *db = pParse->db;
  sqlite3_value *pVal = 0;
  sqlite3ValueFromExpr(db, pExpr, db->enc, SQLITE_AFF_NUMERIC, &pVal);
  if( pVal && sqlite3_value_int(pVal)>0 ){
    ret = 1;
  }
  sqlite3ValueFree(pVal);
  return ret;
}