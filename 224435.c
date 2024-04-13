void sqlite3WindowChain(Parse *pParse, Window *pWin, Window *pList){
  if( pWin->zBase ){
    sqlite3 *db = pParse->db;
    Window *pExist = windowFind(pParse, pList, pWin->zBase);
    if( pExist ){
      const char *zErr = 0;
      /* Check for errors */
      if( pWin->pPartition ){
        zErr = "PARTITION clause";
      }else if( pExist->pOrderBy && pWin->pOrderBy ){
        zErr = "ORDER BY clause";
      }else if( pExist->bImplicitFrame==0 ){
        zErr = "frame specification";
      }
      if( zErr ){
        sqlite3ErrorMsg(pParse, 
            "cannot override %s of window: %s", zErr, pWin->zBase
        );
      }else{
        pWin->pPartition = sqlite3ExprListDup(db, pExist->pPartition, 0);
        if( pExist->pOrderBy ){
          assert( pWin->pOrderBy==0 );
          pWin->pOrderBy = sqlite3ExprListDup(db, pExist->pOrderBy, 0);
        }
        sqlite3DbFree(db, pWin->zBase);
        pWin->zBase = 0;
      }
    }
  }
}