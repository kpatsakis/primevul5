static void rankStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    p->nStep++;
    if( p->nValue==0 ){
      p->nValue = p->nStep;
    }
  }
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
}