static Window *windowFind(Parse *pParse, Window *pList, const char *zName){
  Window *p;
  for(p=pList; p; p=p->pNextWin){
    if( sqlite3StrICmp(p->zName, zName)==0 ) break;
  }
  if( p==0 ){
    sqlite3ErrorMsg(pParse, "no such window: %s", zName);
  }
  return p;
}