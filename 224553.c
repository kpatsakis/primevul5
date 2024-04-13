static void windowFullScan(WindowCodeArg *p){
  Window *pWin;
  Parse *pParse = p->pParse;
  Window *pMWin = p->pMWin;
  Vdbe *v = p->pVdbe;

  int regCRowid = 0;              /* Current rowid value */
  int regCPeer = 0;               /* Current peer values */
  int regRowid = 0;               /* AggStep rowid value */
  int regPeer = 0;                /* AggStep peer values */

  int nPeer;
  int lblNext;
  int lblBrk;
  int addrNext;
  int csr;

  VdbeModuleComment((v, "windowFullScan begin"));

  assert( pMWin!=0 );
  csr = pMWin->csrApp;
  nPeer = (pMWin->pOrderBy ? pMWin->pOrderBy->nExpr : 0);

  lblNext = sqlite3VdbeMakeLabel(pParse);
  lblBrk = sqlite3VdbeMakeLabel(pParse);

  regCRowid = sqlite3GetTempReg(pParse);
  regRowid = sqlite3GetTempReg(pParse);
  if( nPeer ){
    regCPeer = sqlite3GetTempRange(pParse, nPeer);
    regPeer = sqlite3GetTempRange(pParse, nPeer);
  }

  sqlite3VdbeAddOp2(v, OP_Rowid, pMWin->iEphCsr, regCRowid);
  windowReadPeerValues(p, pMWin->iEphCsr, regCPeer);

  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    sqlite3VdbeAddOp2(v, OP_Null, 0, pWin->regAccum);
  }

  sqlite3VdbeAddOp3(v, OP_SeekGE, csr, lblBrk, pMWin->regStartRowid);
  VdbeCoverage(v);
  addrNext = sqlite3VdbeCurrentAddr(v);
  sqlite3VdbeAddOp2(v, OP_Rowid, csr, regRowid);
  sqlite3VdbeAddOp3(v, OP_Gt, pMWin->regEndRowid, lblBrk, regRowid);
  VdbeCoverageNeverNull(v);

  if( pMWin->eExclude==TK_CURRENT ){
    sqlite3VdbeAddOp3(v, OP_Eq, regCRowid, lblNext, regRowid);
    VdbeCoverageNeverNull(v);
  }else if( pMWin->eExclude!=TK_NO ){
    int addr;
    int addrEq = 0;
    KeyInfo *pKeyInfo = 0;

    if( pMWin->pOrderBy ){
      pKeyInfo = sqlite3KeyInfoFromExprList(pParse, pMWin->pOrderBy, 0, 0);
    }
    if( pMWin->eExclude==TK_TIES ){
      addrEq = sqlite3VdbeAddOp3(v, OP_Eq, regCRowid, 0, regRowid);
      VdbeCoverageNeverNull(v);
    }
    if( pKeyInfo ){
      windowReadPeerValues(p, csr, regPeer);
      sqlite3VdbeAddOp3(v, OP_Compare, regPeer, regCPeer, nPeer);
      sqlite3VdbeAppendP4(v, (void*)pKeyInfo, P4_KEYINFO);
      addr = sqlite3VdbeCurrentAddr(v)+1;
      sqlite3VdbeAddOp3(v, OP_Jump, addr, lblNext, addr);
      VdbeCoverageEqNe(v);
    }else{
      sqlite3VdbeAddOp2(v, OP_Goto, 0, lblNext);
    }
    if( addrEq ) sqlite3VdbeJumpHere(v, addrEq);
  }

  windowAggStep(p, pMWin, csr, 0, p->regArg);

  sqlite3VdbeResolveLabel(v, lblNext);
  sqlite3VdbeAddOp2(v, OP_Next, csr, addrNext);
  VdbeCoverage(v);
  sqlite3VdbeJumpHere(v, addrNext-1);
  sqlite3VdbeJumpHere(v, addrNext+1);
  sqlite3ReleaseTempReg(pParse, regRowid);
  sqlite3ReleaseTempReg(pParse, regCRowid);
  if( nPeer ){
    sqlite3ReleaseTempRange(pParse, regPeer, nPeer);
    sqlite3ReleaseTempRange(pParse, regCPeer, nPeer);
  }

  windowAggFinal(p, 1);
  VdbeModuleComment((v, "windowFullScan end"));
}