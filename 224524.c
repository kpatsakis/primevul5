Window *sqlite3WindowAssemble(
  Parse *pParse, 
  Window *pWin, 
  ExprList *pPartition, 
  ExprList *pOrderBy, 
  Token *pBase
){
  if( pWin ){
    pWin->pPartition = pPartition;
    pWin->pOrderBy = pOrderBy;
    if( pBase ){
      pWin->zBase = sqlite3DbStrNDup(pParse->db, pBase->z, pBase->n);
    }
  }else{
    sqlite3ExprListDelete(pParse->db, pPartition);
    sqlite3ExprListDelete(pParse->db, pOrderBy);
  }
  return pWin;
}