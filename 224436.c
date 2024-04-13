void sqlite3VdbeFreeCursor(Vdbe *p, VdbeCursor *pCx){
  if( pCx==0 ){
    return;
  }
  assert( pCx->pBtx==0 || pCx->eCurType==CURTYPE_BTREE );
  switch( pCx->eCurType ){
    case CURTYPE_SORTER: {
      sqlite3VdbeSorterClose(p->db, pCx);
      break;
    }
    case CURTYPE_BTREE: {
      if( pCx->isEphemeral ){
        if( pCx->pBtx ) sqlite3BtreeClose(pCx->pBtx);
        /* The pCx->pCursor will be close automatically, if it exists, by
        ** the call above. */
      }else{
        assert( pCx->uc.pCursor!=0 );
        sqlite3BtreeCloseCursor(pCx->uc.pCursor);
      }
      break;
    }
#ifndef SQLITE_OMIT_VIRTUALTABLE
    case CURTYPE_VTAB: {
      sqlite3_vtab_cursor *pVCur = pCx->uc.pVCur;
      const sqlite3_module *pModule = pVCur->pVtab->pModule;
      assert( pVCur->pVtab->nRef>0 );
      pVCur->pVtab->nRef--;
      pModule->xClose(pVCur);
      break;
    }
#endif
  }
}