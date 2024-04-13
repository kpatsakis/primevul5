static void last_valueStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct LastValueCtx *p;
  UNUSED_PARAMETER(nArg);
  p = (struct LastValueCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    sqlite3_value_free(p->pVal);
    p->pVal = sqlite3_value_dup(apArg[0]);
    if( p->pVal==0 ){
      sqlite3_result_error_nomem(pCtx);
    }else{
      p->nVal++;
    }
  }
}