void sqlite3WindowAttach(Parse *pParse, Expr *p, Window *pWin){
  if( p ){
    assert( p->op==TK_FUNCTION );
    assert( pWin );
    p->y.pWin = pWin;
    ExprSetProperty(p, EP_WinFunc);
    pWin->pOwner = p;
    if( (p->flags & EP_Distinct) && pWin->eFrmType!=TK_FILTER ){
      sqlite3ErrorMsg(pParse,
          "DISTINCT is not supported for window functions"
      );
    }
  }else{
    sqlite3WindowDelete(pParse->db, pWin);
  }
}