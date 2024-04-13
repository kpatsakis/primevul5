static void row_numberStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  i64 *p = (i64*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ) (*p)++;
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
}