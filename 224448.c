static void windowReadPeerValues(
  WindowCodeArg *p,
  int csr,
  int reg
){
  Window *pMWin = p->pMWin;
  ExprList *pOrderBy = pMWin->pOrderBy;
  if( pOrderBy ){
    Vdbe *v = sqlite3GetVdbe(p->pParse);
    ExprList *pPart = pMWin->pPartition;
    int iColOff = pMWin->nBufferCol + (pPart ? pPart->nExpr : 0);
    int i;
    for(i=0; i<pOrderBy->nExpr; i++){
      sqlite3VdbeAddOp3(v, OP_Column, csr, iColOff+i, reg+i);
    }
  }
}