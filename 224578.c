static int SQLITE_NOINLINE handleDeferredMoveto(VdbeCursor *p){
  int res, rc;
#ifdef SQLITE_TEST
  extern int sqlite3_search_count;
#endif
  assert( p->deferredMoveto );
  assert( p->isTable );
  assert( p->eCurType==CURTYPE_BTREE );
  rc = sqlite3BtreeMovetoUnpacked(p->uc.pCursor, 0, p->movetoTarget, 0, &res);
  if( rc ) return rc;
  if( res!=0 ) return SQLITE_CORRUPT_BKPT;
#ifdef SQLITE_TEST
  sqlite3_search_count++;
#endif
  p->deferredMoveto = 0;
  p->cacheStatus = CACHE_STALE;
  return SQLITE_OK;
}