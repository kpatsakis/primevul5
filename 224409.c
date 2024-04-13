static void dense_rankValueFunc(sqlite3_context *pCtx){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    if( p->nStep ){
      p->nValue++;
      p->nStep = 0;
    }
    sqlite3_result_int64(pCtx, p->nValue);
  }
}