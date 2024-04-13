static void last_valueValueFunc(sqlite3_context *pCtx){
  struct LastValueCtx *p;
  p = (struct LastValueCtx*)sqlite3_aggregate_context(pCtx, 0);
  if( p && p->pVal ){
    sqlite3_result_value(pCtx, p->pVal);
  }
}