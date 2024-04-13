static void windowIfNewPeer(
  Parse *pParse,
  ExprList *pOrderBy,
  int regNew,                     /* First in array of new values */
  int regOld,                     /* First in array of old values */
  int addr                        /* Jump here */
){
  Vdbe *v = sqlite3GetVdbe(pParse);
  if( pOrderBy ){
    int nVal = pOrderBy->nExpr;
    KeyInfo *pKeyInfo = sqlite3KeyInfoFromExprList(pParse, pOrderBy, 0, 0);
    sqlite3VdbeAddOp3(v, OP_Compare, regOld, regNew, nVal);
    sqlite3VdbeAppendP4(v, (void*)pKeyInfo, P4_KEYINFO);
    sqlite3VdbeAddOp3(v, OP_Jump, 
      sqlite3VdbeCurrentAddr(v)+1, addr, sqlite3VdbeCurrentAddr(v)+1
    );
    VdbeCoverageEqNe(v);
    sqlite3VdbeAddOp3(v, OP_Copy, regNew, regOld, nVal-1);
  }else{
    sqlite3VdbeAddOp2(v, OP_Goto, 0, addr);
  }
}