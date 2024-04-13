static int windowArgCount(Window *pWin){
  ExprList *pList = pWin->pOwner->x.pList;
  return (pList ? pList->nExpr : 0);
}