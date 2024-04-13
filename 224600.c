static void windowAggStep(
  WindowCodeArg *p,
  Window *pMWin,                  /* Linked list of window functions */
  int csr,                        /* Read arguments from this cursor */
  int bInverse,                   /* True to invoke xInverse instead of xStep */
  int reg                         /* Array of registers */
){
  Parse *pParse = p->pParse;
  Vdbe *v = sqlite3GetVdbe(pParse);
  Window *pWin;
  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    FuncDef *pFunc = pWin->pFunc;
    int regArg;
    int nArg = pWin->bExprArgs ? 0 : windowArgCount(pWin);
    int i;

    assert( bInverse==0 || pWin->eStart!=TK_UNBOUNDED );

    /* All OVER clauses in the same window function aggregate step must
    ** be the same. */
    assert( pWin==pMWin || sqlite3WindowCompare(pParse,pWin,pMWin,0)==0 );

    for(i=0; i<nArg; i++){
      if( i!=1 || pFunc->zName!=nth_valueName ){
        sqlite3VdbeAddOp3(v, OP_Column, csr, pWin->iArgCol+i, reg+i);
      }else{
        sqlite3VdbeAddOp3(v, OP_Column, pMWin->iEphCsr, pWin->iArgCol+i, reg+i);
      }
    }
    regArg = reg;

    if( pMWin->regStartRowid==0
     && (pFunc->funcFlags & SQLITE_FUNC_MINMAX) 
     && (pWin->eStart!=TK_UNBOUNDED)
    ){
      int addrIsNull = sqlite3VdbeAddOp1(v, OP_IsNull, regArg);
      VdbeCoverage(v);
      if( bInverse==0 ){
        sqlite3VdbeAddOp2(v, OP_AddImm, pWin->regApp+1, 1);
        sqlite3VdbeAddOp2(v, OP_SCopy, regArg, pWin->regApp);
        sqlite3VdbeAddOp3(v, OP_MakeRecord, pWin->regApp, 2, pWin->regApp+2);
        sqlite3VdbeAddOp2(v, OP_IdxInsert, pWin->csrApp, pWin->regApp+2);
      }else{
        sqlite3VdbeAddOp4Int(v, OP_SeekGE, pWin->csrApp, 0, regArg, 1);
        VdbeCoverageNeverTaken(v);
        sqlite3VdbeAddOp1(v, OP_Delete, pWin->csrApp);
        sqlite3VdbeJumpHere(v, sqlite3VdbeCurrentAddr(v)-2);
      }
      sqlite3VdbeJumpHere(v, addrIsNull);
    }else if( pWin->regApp ){
      assert( pFunc->zName==nth_valueName
           || pFunc->zName==first_valueName
      );
      assert( bInverse==0 || bInverse==1 );
      sqlite3VdbeAddOp2(v, OP_AddImm, pWin->regApp+1-bInverse, 1);
    }else if( pFunc->xSFunc!=noopStepFunc ){
      int addrIf = 0;
      if( pWin->pFilter ){
        int regTmp;
        assert( pWin->bExprArgs || !nArg ||nArg==pWin->pOwner->x.pList->nExpr );
        assert( pWin->bExprArgs || nArg  ||pWin->pOwner->x.pList==0 );
        regTmp = sqlite3GetTempReg(pParse);
        sqlite3VdbeAddOp3(v, OP_Column, csr, pWin->iArgCol+nArg,regTmp);
        addrIf = sqlite3VdbeAddOp3(v, OP_IfNot, regTmp, 0, 1);
        VdbeCoverage(v);
        sqlite3ReleaseTempReg(pParse, regTmp);
      }
      
      if( pWin->bExprArgs ){
        int iStart = sqlite3VdbeCurrentAddr(v);
        VdbeOp *pOp, *pEnd;

        nArg = pWin->pOwner->x.pList->nExpr;
        regArg = sqlite3GetTempRange(pParse, nArg);
        sqlite3ExprCodeExprList(pParse, pWin->pOwner->x.pList, regArg, 0, 0);

        pEnd = sqlite3VdbeGetOp(v, -1);
        for(pOp=sqlite3VdbeGetOp(v, iStart); pOp<=pEnd; pOp++){
          if( pOp->opcode==OP_Column && pOp->p1==pWin->iEphCsr ){
            pOp->p1 = csr;
          }
        }
      }
      if( pFunc->funcFlags & SQLITE_FUNC_NEEDCOLL ){
        CollSeq *pColl;
        assert( nArg>0 );
        pColl = sqlite3ExprNNCollSeq(pParse, pWin->pOwner->x.pList->a[0].pExpr);
        sqlite3VdbeAddOp4(v, OP_CollSeq, 0,0,0, (const char*)pColl, P4_COLLSEQ);
      }
      sqlite3VdbeAddOp3(v, bInverse? OP_AggInverse : OP_AggStep, 
                        bInverse, regArg, pWin->regAccum);
      sqlite3VdbeAppendP4(v, pFunc, P4_FUNCDEF);
      sqlite3VdbeChangeP5(v, (u8)nArg);
      if( pWin->bExprArgs ){
        sqlite3ReleaseTempRange(pParse, regArg, nArg);
      }
      if( addrIf ) sqlite3VdbeJumpHere(v, addrIf);
    }
  }
}