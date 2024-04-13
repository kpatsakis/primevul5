static void last_valueInvFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct LastValueCtx *p;
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
  p = (struct LastValueCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( ALWAYS(p) ){
    p->nVal--;
    if( p->nVal==0 ){
      sqlite3_value_free(p->pVal);
      p->pVal = 0;
    }
  }
}