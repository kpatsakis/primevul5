static void last_valueFinalizeFunc(sqlite3_context *pCtx){
  struct LastValueCtx *p;
  p = (struct LastValueCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p && p->pVal ){
    sqlite3_result_value(pCtx, p->pVal);
    sqlite3_value_free(p->pVal);
    p->pVal = 0;
  }
}