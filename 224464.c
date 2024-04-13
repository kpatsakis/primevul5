static int windowInitAccum(Parse *pParse, Window *pMWin){
  Vdbe *v = sqlite3GetVdbe(pParse);
  int regArg;
  int nArg = 0;
  Window *pWin;
  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    FuncDef *pFunc = pWin->pFunc;
    sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regAccum);
    nArg = MAX(nArg, windowArgCount(pWin));
    if( pMWin->regStartRowid==0 ){
      if( pFunc->zName==nth_valueName || pFunc->zName==first_valueName ){
        sqlite3VdbeAddOp2(v, OP_Integer, 0, pWin->regApp);
        sqlite3VdbeAddOp2(v, OP_Integer, 0, pWin->regApp+1);
      }

      if( (pFunc->funcFlags & SQLITE_FUNC_MINMAX) && pWin->csrApp ){
        assert( pWin->eStart!=TK_UNBOUNDED );
        sqlite3VdbeAddOp1(v, OP_ResetSorter, pWin->csrApp);
        sqlite3VdbeAddOp2(v, OP_Integer, 0, pWin->regApp+1);
      }
    }
  }
  regArg = pParse->nMem+1;
  pParse->nMem += nArg;
  return regArg;
}