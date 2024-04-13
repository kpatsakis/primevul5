static void windowReturnOneRow(WindowCodeArg *p){
  Window *pMWin = p->pMWin;
  Vdbe *v = p->pVdbe;

  if( pMWin->regStartRowid ){
    windowFullScan(p);
  }else{
    Parse *pParse = p->pParse;
    Window *pWin;

    for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
      FuncDef *pFunc = pWin->pFunc;
      if( pFunc->zName==nth_valueName
       || pFunc->zName==first_valueName
      ){
        int csr = pWin->csrApp;
        int lbl = sqlite3VdbeMakeLabel(pParse);
        int tmpReg = sqlite3GetTempReg(pParse);
        sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regResult);
  
        if( pFunc->zName==nth_valueName ){
          sqlite3VdbeAddOp3(v, OP_Column,pMWin->iEphCsr,pWin->iArgCol+1,tmpReg);
          windowCheckValue(pParse, tmpReg, 2);
        }else{
          sqlite3VdbeAddOp2(v, OP_Integer, 1, tmpReg);
        }
        sqlite3VdbeAddOp3(v, OP_Add, tmpReg, pWin->regApp, tmpReg);
        sqlite3VdbeAddOp3(v, OP_Gt, pWin->regApp+1, lbl, tmpReg);
        VdbeCoverageNeverNull(v);
        sqlite3VdbeAddOp3(v, OP_SeekRowid, csr, 0, tmpReg);
        VdbeCoverageNeverTaken(v);
        sqlite3VdbeAddOp3(v, OP_Column, csr, pWin->iArgCol, pWin->regResult);
        sqlite3VdbeResolveLabel(v, lbl);
        sqlite3ReleaseTempReg(pParse, tmpReg);
      }
      else if( pFunc->zName==leadName || pFunc->zName==lagName ){
        int nArg = pWin->pOwner->x.pList->nExpr;
        int csr = pWin->csrApp;
        int lbl = sqlite3VdbeMakeLabel(pParse);
        int tmpReg = sqlite3GetTempReg(pParse);
        int iEph = pMWin->iEphCsr;
  
        if( nArg<3 ){
          sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regResult);
        }else{
          sqlite3VdbeAddOp3(v, OP_Column, iEph,pWin->iArgCol+2,pWin->regResult);
        }
        sqlite3VdbeAddOp2(v, OP_Rowid, iEph, tmpReg);
        if( nArg<2 ){
          int val = (pFunc->zName==leadName ? 1 : -1);
          sqlite3VdbeAddOp2(v, OP_AddImm, tmpReg, val);
        }else{
          int op = (pFunc->zName==leadName ? OP_Add : OP_Subtract);
          int tmpReg2 = sqlite3GetTempReg(pParse);
          sqlite3VdbeAddOp3(v, OP_Column, iEph, pWin->iArgCol+1, tmpReg2);
          sqlite3VdbeAddOp3(v, op, tmpReg2, tmpReg, tmpReg);
          sqlite3ReleaseTempReg(pParse, tmpReg2);
        }
  
        sqlite3VdbeAddOp3(v, OP_SeekRowid, csr, lbl, tmpReg);
        VdbeCoverage(v);
        sqlite3VdbeAddOp3(v, OP_Column, csr, pWin->iArgCol, pWin->regResult);
        sqlite3VdbeResolveLabel(v, lbl);
        sqlite3ReleaseTempReg(pParse, tmpReg);
      }
    }
  }
  sqlite3VdbeAddOp2(v, OP_Gosub, p->regGosub, p->addrGosub);
}