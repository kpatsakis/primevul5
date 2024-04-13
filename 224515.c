void sqlite3ExprCodeMove(Parse *pParse, int iFrom, int iTo, int nReg){
  sqlite3VdbeAddOp3(pParse->pVdbe, OP_Move, iFrom, iTo, nReg);
}