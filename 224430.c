void sqlite3ExprCodeGeneratedColumn(
  Parse *pParse,
  Column *pCol,
  int regOut
){
  int iAddr;
  Vdbe *v = pParse->pVdbe;
  assert( v!=0 );
  assert( pParse->iSelfTab!=0 );
  if( pParse->iSelfTab>0 ){
    iAddr = sqlite3VdbeAddOp3(v, OP_IfNullRow, pParse->iSelfTab-1, 0, regOut);
  }else{
    iAddr = 0;
  }
  sqlite3ExprCode(pParse, pCol->pDflt, regOut);
  if( pCol->affinity>=SQLITE_AFF_TEXT ){
    sqlite3VdbeAddOp4(v, OP_Affinity, regOut, 1, 0, &pCol->affinity, 1);
  }
  if( iAddr ) sqlite3VdbeJumpHere(v, iAddr);
}