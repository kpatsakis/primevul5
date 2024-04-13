static void cume_distValueFunc(sqlite3_context *pCtx){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, 0);
  if( p ){
    double r = (double)(p->nStep) / (double)(p->nTotal);
    sqlite3_result_double(pCtx, r);
  }
}