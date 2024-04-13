int sqlite3WindowCompare(Parse *pParse, Window *p1, Window *p2, int bFilter){
  if( NEVER(p1==0) || NEVER(p2==0) ) return 1;
  if( p1->eFrmType!=p2->eFrmType ) return 1;
  if( p1->eStart!=p2->eStart ) return 1;
  if( p1->eEnd!=p2->eEnd ) return 1;
  if( p1->eExclude!=p2->eExclude ) return 1;
  if( sqlite3ExprCompare(pParse, p1->pStart, p2->pStart, -1) ) return 1;
  if( sqlite3ExprCompare(pParse, p1->pEnd, p2->pEnd, -1) ) return 1;
  if( sqlite3ExprListCompare(p1->pPartition, p2->pPartition, -1) ) return 1;
  if( sqlite3ExprListCompare(p1->pOrderBy, p2->pOrderBy, -1) ) return 1;
  if( bFilter ){
    if( sqlite3ExprCompare(pParse, p1->pFilter, p2->pFilter, -1) ) return 1;
  }
  return 0;
}