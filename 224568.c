void sqlite3VdbePreUpdateHook(
  Vdbe *v,                        /* Vdbe pre-update hook is invoked by */
  VdbeCursor *pCsr,               /* Cursor to grab old.* values from */
  int op,                         /* SQLITE_INSERT, UPDATE or DELETE */
  const char *zDb,                /* Database name */
  Table *pTab,                    /* Modified table */
  i64 iKey1,                      /* Initial key value */
  int iReg                        /* Register for new.* record */
){
  sqlite3 *db = v->db;
  i64 iKey2;
  PreUpdate preupdate;
  const char *zTbl = pTab->zName;
  static const u8 fakeSortOrder = 0;

  assert( db->pPreUpdate==0 );
  memset(&preupdate, 0, sizeof(PreUpdate));
  if( HasRowid(pTab)==0 ){
    iKey1 = iKey2 = 0;
    preupdate.pPk = sqlite3PrimaryKeyIndex(pTab);
  }else{
    if( op==SQLITE_UPDATE ){
      iKey2 = v->aMem[iReg].u.i;
    }else{
      iKey2 = iKey1;
    }
  }

  assert( pCsr->nField==pTab->nCol 
       || (pCsr->nField==pTab->nCol+1 && op==SQLITE_DELETE && iReg==-1)
  );

  preupdate.v = v;
  preupdate.pCsr = pCsr;
  preupdate.op = op;
  preupdate.iNewReg = iReg;
  preupdate.keyinfo.db = db;
  preupdate.keyinfo.enc = ENC(db);
  preupdate.keyinfo.nKeyField = pTab->nCol;
  preupdate.keyinfo.aSortFlags = (u8*)&fakeSortOrder;
  preupdate.iKey1 = iKey1;
  preupdate.iKey2 = iKey2;
  preupdate.pTab = pTab;

  db->pPreUpdate = &preupdate;
  db->xPreUpdateCallback(db->pPreUpdateArg, db, op, zDb, zTbl, iKey1, iKey2);
  db->pPreUpdate = 0;
  sqlite3DbFree(db, preupdate.aRecord);
  vdbeFreeUnpacked(db, preupdate.keyinfo.nKeyField+1, preupdate.pUnpacked);
  vdbeFreeUnpacked(db, preupdate.keyinfo.nKeyField+1, preupdate.pNewUnpacked);
  if( preupdate.aNew ){
    int i;
    for(i=0; i<pCsr->nField; i++){
      sqlite3VdbeMemRelease(&preupdate.aNew[i]);
    }
    sqlite3DbFreeNN(db, preupdate.aNew);
  }
}