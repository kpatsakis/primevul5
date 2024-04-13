static void vdbeFreeUnpacked(sqlite3 *db, int nField, UnpackedRecord *p){
  if( p ){
    int i;
    for(i=0; i<nField; i++){
      Mem *pMem = &p->aMem[i];
      if( pMem->zMalloc ) sqlite3VdbeMemRelease(pMem);
    }
    sqlite3DbFreeNN(db, p);
  }
}