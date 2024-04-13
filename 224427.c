static void row_numberValueFunc(sqlite3_context *pCtx){
  i64 *p = (i64*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  sqlite3_result_int64(pCtx, (p ? *p : 0));
}