static void rankValueFunc(sqlite3_context *pCtx){
  struct CallCount *p;
  p = (struct CallCount*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    sqlite3_result_int64(pCtx, p->nValue);
    p->nValue = 0;
  }
}