static void generateSortTail(
  Parse *pParse,    /* Parsing context */
  Select *p,        /* The SELECT statement */
  SortCtx *pSort,   /* Information on the ORDER BY clause */
  int nColumn,      /* Number of columns of data */
  SelectDest *pDest /* Write the sorted results here */
){
  Vdbe *v = pParse->pVdbe;                     /* The prepared statement */
  int addrBreak = pSort->labelDone;            /* Jump here to exit loop */
  int addrContinue = sqlite3VdbeMakeLabel(pParse);/* Jump here for next cycle */
  int addr;                       /* Top of output loop. Jump for Next. */
  int addrOnce = 0;
  int iTab;
  ExprList *pOrderBy = pSort->pOrderBy;
  int eDest = pDest->eDest;
  int iParm = pDest->iSDParm;
  int regRow;
  int regRowid;
  int iCol;
  int nKey;                       /* Number of key columns in sorter record */
  int iSortTab;                   /* Sorter cursor to read from */
  int i;
  int bSeq;                       /* True if sorter record includes seq. no. */
  int nRefKey = 0;
  struct ExprList_item *aOutEx = p->pEList->a;

  assert( addrBreak<0 );
  if( pSort->labelBkOut ){
    sqlite3VdbeAddOp2(v, OP_Gosub, pSort->regReturn, pSort->labelBkOut);
    sqlite3VdbeGoto(v, addrBreak);
    sqlite3VdbeResolveLabel(v, pSort->labelBkOut);
  }

#ifdef SQLITE_ENABLE_SORTER_REFERENCES
  /* Open any cursors needed for sorter-reference expressions */
  for(i=0; i<pSort->nDefer; i++){
    Table *pTab = pSort->aDefer[i].pTab;
    int iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
    sqlite3OpenTable(pParse, pSort->aDefer[i].iCsr, iDb, pTab, OP_OpenRead);
    nRefKey = MAX(nRefKey, pSort->aDefer[i].nKey);
  }
#endif

  iTab = pSort->iECursor;
  if( eDest==SRT_Output || eDest==SRT_Coroutine || eDest==SRT_Mem ){
    regRowid = 0;
    regRow = pDest->iSdst;
  }else{
    regRowid = sqlite3GetTempReg(pParse);
    if( eDest==SRT_EphemTab || eDest==SRT_Table ){
      regRow = sqlite3GetTempReg(pParse);
      nColumn = 0;
    }else{
      regRow = sqlite3GetTempRange(pParse, nColumn);
    }
  }
  nKey = pOrderBy->nExpr - pSort->nOBSat;
  if( pSort->sortFlags & SORTFLAG_UseSorter ){
    int regSortOut = ++pParse->nMem;
    iSortTab = pParse->nTab++;
    if( pSort->labelBkOut ){
      addrOnce = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
    }
    sqlite3VdbeAddOp3(v, OP_OpenPseudo, iSortTab, regSortOut, 
        nKey+1+nColumn+nRefKey);
    if( addrOnce ) sqlite3VdbeJumpHere(v, addrOnce);
    addr = 1 + sqlite3VdbeAddOp2(v, OP_SorterSort, iTab, addrBreak);
    VdbeCoverage(v);
    codeOffset(v, p->iOffset, addrContinue);
    sqlite3VdbeAddOp3(v, OP_SorterData, iTab, regSortOut, iSortTab);
    bSeq = 0;
  }else{
    addr = 1 + sqlite3VdbeAddOp2(v, OP_Sort, iTab, addrBreak); VdbeCoverage(v);
    codeOffset(v, p->iOffset, addrContinue);
    iSortTab = iTab;
    bSeq = 1;
  }
  for(i=0, iCol=nKey+bSeq-1; i<nColumn; i++){
#ifdef SQLITE_ENABLE_SORTER_REFERENCES
    if( aOutEx[i].bSorterRef ) continue;
#endif
    if( aOutEx[i].u.x.iOrderByCol==0 ) iCol++;
  }
#ifdef SQLITE_ENABLE_SORTER_REFERENCES
  if( pSort->nDefer ){
    int iKey = iCol+1;
    int regKey = sqlite3GetTempRange(pParse, nRefKey);

    for(i=0; i<pSort->nDefer; i++){
      int iCsr = pSort->aDefer[i].iCsr;
      Table *pTab = pSort->aDefer[i].pTab;
      int nKey = pSort->aDefer[i].nKey;

      sqlite3VdbeAddOp1(v, OP_NullRow, iCsr);
      if( HasRowid(pTab) ){
        sqlite3VdbeAddOp3(v, OP_Column, iSortTab, iKey++, regKey);
        sqlite3VdbeAddOp3(v, OP_SeekRowid, iCsr, 
            sqlite3VdbeCurrentAddr(v)+1, regKey);
      }else{
        int k;
        int iJmp;
        assert( sqlite3PrimaryKeyIndex(pTab)->nKeyCol==nKey );
        for(k=0; k<nKey; k++){
          sqlite3VdbeAddOp3(v, OP_Column, iSortTab, iKey++, regKey+k);
        }
        iJmp = sqlite3VdbeCurrentAddr(v);
        sqlite3VdbeAddOp4Int(v, OP_SeekGE, iCsr, iJmp+2, regKey, nKey);
        sqlite3VdbeAddOp4Int(v, OP_IdxLE, iCsr, iJmp+3, regKey, nKey);
        sqlite3VdbeAddOp1(v, OP_NullRow, iCsr);
      }
    }
    sqlite3ReleaseTempRange(pParse, regKey, nRefKey);
  }
#endif
  for(i=nColumn-1; i>=0; i--){
#ifdef SQLITE_ENABLE_SORTER_REFERENCES
    if( aOutEx[i].bSorterRef ){
      sqlite3ExprCode(pParse, aOutEx[i].pExpr, regRow+i);
    }else
#endif
    {
      int iRead;
      if( aOutEx[i].u.x.iOrderByCol ){
        iRead = aOutEx[i].u.x.iOrderByCol-1;
      }else{
        iRead = iCol--;
      }
      sqlite3VdbeAddOp3(v, OP_Column, iSortTab, iRead, regRow+i);
      VdbeComment((v, "%s", aOutEx[i].zName?aOutEx[i].zName : aOutEx[i].zSpan));
    }
  }
  switch( eDest ){
    case SRT_Table:
    case SRT_EphemTab: {
      sqlite3VdbeAddOp3(v, OP_Column, iSortTab, nKey+bSeq, regRow);
      sqlite3VdbeAddOp2(v, OP_NewRowid, iParm, regRowid);
      sqlite3VdbeAddOp3(v, OP_Insert, iParm, regRow, regRowid);
      sqlite3VdbeChangeP5(v, OPFLAG_APPEND);
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case SRT_Set: {
      assert( nColumn==sqlite3Strlen30(pDest->zAffSdst) );
      sqlite3VdbeAddOp4(v, OP_MakeRecord, regRow, nColumn, regRowid,
                        pDest->zAffSdst, nColumn);
      sqlite3VdbeAddOp4Int(v, OP_IdxInsert, iParm, regRowid, regRow, nColumn);
      break;
    }
    case SRT_Mem: {
      /* The LIMIT clause will terminate the loop for us */
      break;
    }
#endif
    default: {
      assert( eDest==SRT_Output || eDest==SRT_Coroutine ); 
      testcase( eDest==SRT_Output );
      testcase( eDest==SRT_Coroutine );
      if( eDest==SRT_Output ){
        sqlite3VdbeAddOp2(v, OP_ResultRow, pDest->iSdst, nColumn);
      }else{
        sqlite3VdbeAddOp1(v, OP_Yield, pDest->iSDParm);
      }
      break;
    }
  }
  if( regRowid ){
    if( eDest==SRT_Set ){
      sqlite3ReleaseTempRange(pParse, regRow, nColumn);
    }else{
      sqlite3ReleaseTempReg(pParse, regRow);
    }
    sqlite3ReleaseTempReg(pParse, regRowid);
  }
  /* The bottom of the loop
  */
  sqlite3VdbeResolveLabel(v, addrContinue);
  if( pSort->sortFlags & SORTFLAG_UseSorter ){
    sqlite3VdbeAddOp2(v, OP_SorterNext, iTab, addr); VdbeCoverage(v);
  }else{
    sqlite3VdbeAddOp2(v, OP_Next, iTab, addr); VdbeCoverage(v);
  }
  if( pSort->regReturn ) sqlite3VdbeAddOp1(v, OP_Return, pSort->regReturn);
  sqlite3VdbeResolveLabel(v, addrBreak);
}