static int selectWindowRewriteSelectCb(Walker *pWalker, Select *pSelect){
  struct WindowRewrite *p = pWalker->u.pRewrite;
  Select *pSave = p->pSubSelect;
  if( pSave==pSelect ){
    return WRC_Continue;
  }else{
    p->pSubSelect = pSelect;
    sqlite3WalkSelect(pWalker, pSelect);
    p->pSubSelect = pSave;
  }
  return WRC_Prune;
}