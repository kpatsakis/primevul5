Window *sqlite3WindowListDup(sqlite3 *db, Window *p){
  Window *pWin;
  Window *pRet = 0;
  Window **pp = &pRet;

  for(pWin=p; pWin; pWin=pWin->pNextWin){
    *pp = sqlite3WindowDup(db, 0, pWin);
    if( *pp==0 ) break;
    pp = &((*pp)->pNextWin);
  }

  return pRet;
}