void sqlite3WindowCodeInit(Parse *pParse, Window *pMWin){
  Window *pWin;
  Vdbe *v = sqlite3GetVdbe(pParse);

  /* Allocate registers to use for PARTITION BY values, if any. Initialize
  ** said registers to NULL.  */
  if( pMWin->pPartition ){
    int nExpr = pMWin->pPartition->nExpr;
    pMWin->regPart = pParse->nMem+1;
    pParse->nMem += nExpr;
    sqlite3VdbeAddOp3(v, OP_Null, 0, pMWin->regPart, pMWin->regPart+nExpr-1);
  }

  pMWin->regOne = ++pParse->nMem;
  sqlite3VdbeAddOp2(v, OP_Integer, 1, pMWin->regOne);

  if( pMWin->eExclude ){
    pMWin->regStartRowid = ++pParse->nMem;
    pMWin->regEndRowid = ++pParse->nMem;
    pMWin->csrApp = pParse->nTab++;
    sqlite3VdbeAddOp2(v, OP_Integer, 1, pMWin->regStartRowid);
    sqlite3VdbeAddOp2(v, OP_Integer, 0, pMWin->regEndRowid);
    sqlite3VdbeAddOp2(v, OP_OpenDup, pMWin->csrApp, pMWin->iEphCsr);
    return;
  }

  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    FuncDef *p = pWin->pFunc;
    if( (p->funcFlags & SQLITE_FUNC_MINMAX) && pWin->eStart!=TK_UNBOUNDED ){
      /* The inline versions of min() and max() require a single ephemeral
      ** table and 3 registers. The registers are used as follows:
      **
      **   regApp+0: slot to copy min()/max() argument to for MakeRecord
      **   regApp+1: integer value used to ensure keys are unique
      **   regApp+2: output of MakeRecord
      */
      ExprList *pList = pWin->pOwner->x.pList;
      KeyInfo *pKeyInfo = sqlite3KeyInfoFromExprList(pParse, pList, 0, 0);
      pWin->csrApp = pParse->nTab++;
      pWin->regApp = pParse->nMem+1;
      pParse->nMem += 3;
      if( pKeyInfo && pWin->pFunc->zName[1]=='i' ){
        assert( pKeyInfo->aSortFlags[0]==0 );
        pKeyInfo->aSortFlags[0] = KEYINFO_ORDER_DESC;
      }
      sqlite3VdbeAddOp2(v, OP_OpenEphemeral, pWin->csrApp, 2);
      sqlite3VdbeAppendP4(v, pKeyInfo, P4_KEYINFO);
      sqlite3VdbeAddOp2(v, OP_Integer, 0, pWin->regApp+1);
    }
    else if( p->zName==nth_valueName || p->zName==first_valueName ){
      /* Allocate two registers at pWin->regApp. These will be used to
      ** store the start and end index of the current frame.  */
      pWin->regApp = pParse->nMem+1;
      pWin->csrApp = pParse->nTab++;
      pParse->nMem += 2;
      sqlite3VdbeAddOp2(v, OP_OpenDup, pWin->csrApp, pMWin->iEphCsr);
    }
    else if( p->zName==leadName || p->zName==lagName ){
      pWin->csrApp = pParse->nTab++;
      sqlite3VdbeAddOp2(v, OP_OpenDup, pWin->csrApp, pMWin->iEphCsr);
    }
  }
}