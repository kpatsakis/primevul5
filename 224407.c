void sqlite3WindowLink(Select *pSel, Window *pWin){
  if( pSel!=0
   && (0==pSel->pWin || 0==sqlite3WindowCompare(0, pSel->pWin, pWin, 0))
  ){
    pWin->pNextWin = pSel->pWin;
    if( pSel->pWin ){
      pSel->pWin->ppThis = &pWin->pNextWin;
    }
    pSel->pWin = pWin;
    pWin->ppThis = &pSel->pWin;
  }
}