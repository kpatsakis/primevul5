static void first_valueStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct NthValueCtx *p;
  p = (struct NthValueCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p && p->pValue==0 ){
    p->pValue = sqlite3_value_dup(apArg[0]);
    if( !p->pValue ){
      sqlite3_result_error_nomem(pCtx);
    }
  }
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
}