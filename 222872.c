void sqlite3SelectReset(Parse *pParse, Select *p){
  if( ALWAYS(p) ){
    clearSelect(pParse->db, p, 0);
    memset(&p->iLimit, 0, sizeof(Select) - offsetof(Select,iLimit));
    p->pEList = sqlite3ExprListAppend(pParse, 0,
                     sqlite3ExprAlloc(pParse->db,TK_NULL,0,0));
  }
}