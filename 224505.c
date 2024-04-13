static void percent_rankValueFunc(sqlite3_context *pCtx){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    p->nValue = p->nStep;
    if( p->nTotal>1 ){
      double r = (double)p->nValue / (double)(p->nTotal-1);
      sqlite3_result_double(pCtx, r);
    }else{
      sqlite3_result_double(pCtx, 0.0);
    }
  }
}