static void nth_valueFinalizeFunc(sqlite3_context *pCtx){
  struct NthValueCtx *p;
  p = (struct NthValueCtx*)sqlite3_aggregate_context(pCtx, 0);
  if( p && p->pValue ){
    sqlite3_result_value(pCtx, p->pValue);
    sqlite3_value_free(p->pValue);
    p->pValue = 0;
  }
}