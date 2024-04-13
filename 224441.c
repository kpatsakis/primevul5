static void ntileValueFunc(sqlite3_context *pCtx){
  struct NtileCtx *p;
  p = (struct NtileCtx*)sqlite3_aggregate_context(pCtx, sizeof(*p));
  if( p && p->nParam>0 ){
    int nSize = (p->nTotal / p->nParam);
    if( nSize==0 ){
      sqlite3_result_int64(pCtx, p->iRow+1);
    }else{
      i64 nLarge = p->nTotal - p->nParam*nSize;
      i64 iSmall = nLarge*(nSize+1);
      i64 iRow = p->iRow;

      assert( (nLarge*(nSize+1) + (p->nParam-nLarge)*nSize)==p->nTotal );

      if( iRow<iSmall ){
        sqlite3_result_int64(pCtx, 1 + iRow/(nSize+1));
      }else{
        sqlite3_result_int64(pCtx, 1 + nLarge + (iRow-iSmall)/nSize);
      }
    }
  }
}