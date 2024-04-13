static int selectWindowRewriteExprCb(Walker *pWalker, Expr *pExpr){
  struct WindowRewrite *p = pWalker->u.pRewrite;
  Parse *pParse = pWalker->pParse;
  assert( p!=0 );
  assert( p->pWin!=0 );

  /* If this function is being called from within a scalar sub-select
  ** that used by the SELECT statement being processed, only process
  ** TK_COLUMN expressions that refer to it (the outer SELECT). Do
  ** not process aggregates or window functions at all, as they belong
  ** to the scalar sub-select.  */
  if( p->pSubSelect ){
    if( pExpr->op!=TK_COLUMN ){
      return WRC_Continue;
    }else{
      int nSrc = p->pSrc->nSrc;
      int i;
      for(i=0; i<nSrc; i++){
        if( pExpr->iTable==p->pSrc->a[i].iCursor ) break;
      }
      if( i==nSrc ) return WRC_Continue;
    }
  }

  switch( pExpr->op ){

    case TK_FUNCTION:
      if( !ExprHasProperty(pExpr, EP_WinFunc) ){
        break;
      }else{
        Window *pWin;
        for(pWin=p->pWin; pWin; pWin=pWin->pNextWin){
          if( pExpr->y.pWin==pWin ){
            assert( pWin->pOwner==pExpr );
            return WRC_Prune;
          }
        }
      }
      /* Fall through.  */

    case TK_AGG_FUNCTION:
    case TK_COLUMN: {
      int iCol = -1;
      if( p->pSub ){
        int i;
        for(i=0; i<p->pSub->nExpr; i++){
          if( 0==sqlite3ExprCompare(0, p->pSub->a[i].pExpr, pExpr, -1) ){
            iCol = i;
            break;
          }
        }
      }
      if( iCol<0 ){
        Expr *pDup = sqlite3ExprDup(pParse->db, pExpr, 0);
        p->pSub = sqlite3ExprListAppend(pParse, p->pSub, pDup);
      }
      if( p->pSub ){
        assert( ExprHasProperty(pExpr, EP_Static)==0 );
        ExprSetProperty(pExpr, EP_Static);
        sqlite3ExprDelete(pParse->db, pExpr);
        ExprClearProperty(pExpr, EP_Static);
        memset(pExpr, 0, sizeof(Expr));

        pExpr->op = TK_COLUMN;
        pExpr->iColumn = (iCol<0 ? p->pSub->nExpr-1: iCol);
        pExpr->iTable = p->pWin->iEphCsr;
        pExpr->y.pTab = p->pTab;
      }
      if( pParse->db->mallocFailed ) return WRC_Abort;
      break;
    }

    default: /* no-op */
      break;
  }

  return WRC_Continue;
}