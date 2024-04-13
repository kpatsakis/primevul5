static void ntileInvFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct NtileCtx *p;
  assert( nArg==1 ); UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
  p = (struct NtileCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  p->iRow++;
}