int sqlite3VdbeCursorRestore(VdbeCursor *p){
  assert( p->eCurType==CURTYPE_BTREE );
  if( sqlite3BtreeCursorHasMoved(p->uc.pCursor) ){
    return handleMovedCursor(p);
  }
  return SQLITE_OK;
}