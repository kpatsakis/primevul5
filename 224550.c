static void cume_distStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct CallCount *p;
  UNUSED_PARAMETER(nArg); assert( nArg==0 );
  UNUSED_PARAMETER(apArg);
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    p->nTotal++;
  }
}