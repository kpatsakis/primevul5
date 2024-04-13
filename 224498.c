static void ntileStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct NtileCtx *p;
  assert( nArg==1 ); UNUSED_PARAMETER(nArg);
  p = (struct NtileCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    if( p->nTotal==0 ){
      p->nParam = sqlite3_value_int64(apArg[0]);
      if( p->nParam<=0 ){
        sqlite3_result_error(
            pCtx, "argument of ntile must be a positive integer", -1
        );
      }
    }
    p->nTotal++;
  }
}