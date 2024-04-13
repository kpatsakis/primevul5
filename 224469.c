void sqlite3WindowUnlinkFromSelect(Window *p){
  if( p->ppThis ){
    *p->ppThis = p->pNextWin;
    if( p->pNextWin ) p->pNextWin->ppThis = p->ppThis;
    p->ppThis = 0;
  }
}