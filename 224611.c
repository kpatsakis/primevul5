void sqlite3VdbeScanStatus(
  Vdbe *p,                        /* VM to add scanstatus() to */
  int addrExplain,                /* Address of OP_Explain (or 0) */
  int addrLoop,                   /* Address of loop counter */ 
  int addrVisit,                  /* Address of rows visited counter */
  LogEst nEst,                    /* Estimated number of output rows */
  const char *zName               /* Name of table or index being scanned */
){
  sqlite3_int64 nByte = (p->nScan+1) * sizeof(ScanStatus);
  ScanStatus *aNew;
  aNew = (ScanStatus*)sqlite3DbRealloc(p->db, p->aScan, nByte);
  if( aNew ){
    ScanStatus *pNew = &aNew[p->nScan++];
    pNew->addrExplain = addrExplain;
    pNew->addrLoop = addrLoop;
    pNew->addrVisit = addrVisit;
    pNew->nEst = nEst;
    pNew->zName = sqlite3DbStrDup(p->db, zName);
    p->aScan = aNew;
  }
}