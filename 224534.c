void sqlite3WindowUpdate(
  Parse *pParse, 
  Window *pList,                  /* List of named windows for this SELECT */
  Window *pWin,                   /* Window frame to update */
  FuncDef *pFunc                  /* Window function definition */
){
  if( pWin->zName && pWin->eFrmType==0 ){
    Window *p = windowFind(pParse, pList, pWin->zName);
    if( p==0 ) return;
    pWin->pPartition = sqlite3ExprListDup(pParse->db, p->pPartition, 0);
    pWin->pOrderBy = sqlite3ExprListDup(pParse->db, p->pOrderBy, 0);
    pWin->pStart = sqlite3ExprDup(pParse->db, p->pStart, 0);
    pWin->pEnd = sqlite3ExprDup(pParse->db, p->pEnd, 0);
    pWin->eStart = p->eStart;
    pWin->eEnd = p->eEnd;
    pWin->eFrmType = p->eFrmType;
    pWin->eExclude = p->eExclude;
  }else{
    sqlite3WindowChain(pParse, pWin, pList);
  }
  if( (pWin->eFrmType==TK_RANGE)
   && (pWin->pStart || pWin->pEnd) 
   && (pWin->pOrderBy==0 || pWin->pOrderBy->nExpr!=1)
  ){
    sqlite3ErrorMsg(pParse, 
      "RANGE with offset PRECEDING/FOLLOWING requires one ORDER BY expression"
    );
  }else
  if( pFunc->funcFlags & SQLITE_FUNC_WINDOW ){
    sqlite3 *db = pParse->db;
    if( pWin->pFilter ){
      sqlite3ErrorMsg(pParse, 
          "FILTER clause may only be used with aggregate window functions"
      );
    }else{
      struct WindowUpdate {
        const char *zFunc;
        int eFrmType;
        int eStart;
        int eEnd;
      } aUp[] = {
        { row_numberName,   TK_ROWS,   TK_UNBOUNDED, TK_CURRENT }, 
        { dense_rankName,   TK_RANGE,  TK_UNBOUNDED, TK_CURRENT }, 
        { rankName,         TK_RANGE,  TK_UNBOUNDED, TK_CURRENT }, 
        { percent_rankName, TK_GROUPS, TK_CURRENT,   TK_UNBOUNDED }, 
        { cume_distName,    TK_GROUPS, TK_FOLLOWING, TK_UNBOUNDED }, 
        { ntileName,        TK_ROWS,   TK_CURRENT,   TK_UNBOUNDED }, 
        { leadName,         TK_ROWS,   TK_UNBOUNDED, TK_UNBOUNDED }, 
        { lagName,          TK_ROWS,   TK_UNBOUNDED, TK_CURRENT }, 
      };
      int i;
      for(i=0; i<ArraySize(aUp); i++){
        if( pFunc->zName==aUp[i].zFunc ){
          sqlite3ExprDelete(db, pWin->pStart);
          sqlite3ExprDelete(db, pWin->pEnd);
          pWin->pEnd = pWin->pStart = 0;
          pWin->eFrmType = aUp[i].eFrmType;
          pWin->eStart = aUp[i].eStart;
          pWin->eEnd = aUp[i].eEnd;
          pWin->eExclude = 0;
          if( pWin->eStart==TK_FOLLOWING ){
            pWin->pStart = sqlite3Expr(db, TK_INTEGER, "1");
          }
          break;
        }
      }
    }
  }
  pWin->pFunc = pFunc;
}