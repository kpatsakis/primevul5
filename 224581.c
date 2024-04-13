static void windowAggFinal(WindowCodeArg *p, int bFin){
  Parse *pParse = p->pParse;
  Window *pMWin = p->pMWin;
  Vdbe *v = sqlite3GetVdbe(pParse);
  Window *pWin;

  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    if( pMWin->regStartRowid==0
     && (pWin->pFunc->funcFlags & SQLITE_FUNC_MINMAX) 
     && (pWin->eStart!=TK_UNBOUNDED)
    ){
      sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regResult);
      sqlite3VdbeAddOp1(v, OP_Last, pWin->csrApp);
      VdbeCoverage(v);
      sqlite3VdbeAddOp3(v, OP_Column, pWin->csrApp, 0, pWin->regResult);
      sqlite3VdbeJumpHere(v, sqlite3VdbeCurrentAddr(v)-2);
    }else if( pWin->regApp ){
      assert( pMWin->regStartRowid==0 );
    }else{
      int nArg = windowArgCount(pWin);
      if( bFin ){
        sqlite3VdbeAddOp2(v, OP_AggFinal, pWin->regAccum, nArg);
        sqlite3VdbeAppendP4(v, pWin->pFunc, P4_FUNCDEF);
        sqlite3VdbeAddOp2(v, OP_Copy, pWin->regAccum, pWin->regResult);
        sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regAccum);
      }else{
        sqlite3VdbeAddOp3(v, OP_AggValue,pWin->regAccum,nArg,pWin->regResult);
        sqlite3VdbeAppendP4(v, pWin->pFunc, P4_FUNCDEF);
      }
    }
  }
}