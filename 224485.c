static void selectWindowRewriteEList(
  Parse *pParse, 
  Window *pWin,
  SrcList *pSrc,
  ExprList *pEList,               /* Rewrite expressions in this list */
  Table *pTab,
  ExprList **ppSub                /* IN/OUT: Sub-select expression-list */
){
  Walker sWalker;
  WindowRewrite sRewrite;

  assert( pWin!=0 );
  memset(&sWalker, 0, sizeof(Walker));
  memset(&sRewrite, 0, sizeof(WindowRewrite));

  sRewrite.pSub = *ppSub;
  sRewrite.pWin = pWin;
  sRewrite.pSrc = pSrc;
  sRewrite.pTab = pTab;

  sWalker.pParse = pParse;
  sWalker.xExprCallback = selectWindowRewriteExprCb;
  sWalker.xSelectCallback = selectWindowRewriteSelectCb;
  sWalker.u.pRewrite = &sRewrite;

  (void)sqlite3WalkExprList(&sWalker, pEList);

  *ppSub = sRewrite.pSub;
}