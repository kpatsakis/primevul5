static void dense_rankStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ) p->nStep = 1;
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
}