static void nth_valueStepFunc(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **apArg
){
  struct NthValueCtx *p;
  p = (struct NthValueCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p ){
    i64 iVal;
    switch( sqlite3_value_numeric_type(apArg[1]) ){
      case SQLITE_INTEGER:
        iVal = sqlite3_value_int64(apArg[1]);
        break;
      case SQLITE_FLOAT: {
        double fVal = sqlite3_value_double(apArg[1]);
        if( ((i64)fVal)!=fVal ) goto error_out;
        iVal = (i64)fVal;
        break;
      }
      default:
        goto error_out;
    }
    if( iVal<=0 ) goto error_out;

    p->nStep++;
    if( iVal==p->nStep ){
      p->pValue = sqlite3_value_dup(apArg[0]);
      if( !p->pValue ){
        sqlite3_result_error_nomem(pCtx);
      }
    }
  }
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(apArg);
  return;

 error_out:
  sqlite3_result_error(
      pCtx, "second argument to nth_value must be a positive integer", -1
  );
}