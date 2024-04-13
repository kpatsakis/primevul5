Window *sqlite3WindowAlloc(
  Parse *pParse,    /* Parsing context */
  int eType,        /* Frame type. TK_RANGE, TK_ROWS, TK_GROUPS, or 0 */
  int eStart,       /* Start type: CURRENT, PRECEDING, FOLLOWING, UNBOUNDED */
  Expr *pStart,     /* Start window size if TK_PRECEDING or FOLLOWING */
  int eEnd,         /* End type: CURRENT, FOLLOWING, TK_UNBOUNDED, PRECEDING */
  Expr *pEnd,       /* End window size if TK_FOLLOWING or PRECEDING */
  u8 eExclude       /* EXCLUDE clause */
){
  Window *pWin = 0;
  int bImplicitFrame = 0;

  /* Parser assures the following: */
  assert( eType==0 || eType==TK_RANGE || eType==TK_ROWS || eType==TK_GROUPS );
  assert( eStart==TK_CURRENT || eStart==TK_PRECEDING
           || eStart==TK_UNBOUNDED || eStart==TK_FOLLOWING );
  assert( eEnd==TK_CURRENT || eEnd==TK_FOLLOWING
           || eEnd==TK_UNBOUNDED || eEnd==TK_PRECEDING );
  assert( (eStart==TK_PRECEDING || eStart==TK_FOLLOWING)==(pStart!=0) );
  assert( (eEnd==TK_FOLLOWING || eEnd==TK_PRECEDING)==(pEnd!=0) );

  if( eType==0 ){
    bImplicitFrame = 1;
    eType = TK_RANGE;
  }

  /* Additionally, the
  ** starting boundary type may not occur earlier in the following list than
  ** the ending boundary type:
  **
  **   UNBOUNDED PRECEDING
  **   <expr> PRECEDING
  **   CURRENT ROW
  **   <expr> FOLLOWING
  **   UNBOUNDED FOLLOWING
  **
  ** The parser ensures that "UNBOUNDED PRECEDING" cannot be used as an ending
  ** boundary, and than "UNBOUNDED FOLLOWING" cannot be used as a starting
  ** frame boundary.
  */
  if( (eStart==TK_CURRENT && eEnd==TK_PRECEDING)
   || (eStart==TK_FOLLOWING && (eEnd==TK_PRECEDING || eEnd==TK_CURRENT))
  ){
    sqlite3ErrorMsg(pParse, "unsupported frame specification");
    goto windowAllocErr;
  }

  pWin = (Window*)sqlite3DbMallocZero(pParse->db, sizeof(Window));
  if( pWin==0 ) goto windowAllocErr;
  pWin->eFrmType = eType;
  pWin->eStart = eStart;
  pWin->eEnd = eEnd;
  if( eExclude==0 && OptimizationDisabled(pParse->db, SQLITE_WindowFunc) ){
    eExclude = TK_NO;
  }
  pWin->eExclude = eExclude;
  pWin->bImplicitFrame = bImplicitFrame;
  pWin->pEnd = sqlite3WindowOffsetExpr(pParse, pEnd);
  pWin->pStart = sqlite3WindowOffsetExpr(pParse, pStart);
  return pWin;

windowAllocErr:
  sqlite3ExprDelete(pParse->db, pEnd);
  sqlite3ExprDelete(pParse->db, pStart);
  return 0;
}