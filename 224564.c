static int windowCodeOp(
 WindowCodeArg *p,                /* Context object */
 int op,                          /* WINDOW_RETURN_ROW, AGGSTEP or AGGINVERSE */
 int regCountdown,                /* Register for OP_IfPos countdown */
 int jumpOnEof                    /* Jump here if stepped cursor reaches EOF */
){
  int csr, reg;
  Parse *pParse = p->pParse;
  Window *pMWin = p->pMWin;
  int ret = 0;
  Vdbe *v = p->pVdbe;
  int addrContinue = 0;
  int bPeer = (pMWin->eFrmType!=TK_ROWS);

  int lblDone = sqlite3VdbeMakeLabel(pParse);
  int addrNextRange = 0;

  /* Special case - WINDOW_AGGINVERSE is always a no-op if the frame
  ** starts with UNBOUNDED PRECEDING. */
  if( op==WINDOW_AGGINVERSE && pMWin->eStart==TK_UNBOUNDED ){
    assert( regCountdown==0 && jumpOnEof==0 );
    return 0;
  }

  if( regCountdown>0 ){
    if( pMWin->eFrmType==TK_RANGE ){
      addrNextRange = sqlite3VdbeCurrentAddr(v);
      assert( op==WINDOW_AGGINVERSE || op==WINDOW_AGGSTEP );
      if( op==WINDOW_AGGINVERSE ){
        if( pMWin->eStart==TK_FOLLOWING ){
          windowCodeRangeTest(
              p, OP_Le, p->current.csr, regCountdown, p->start.csr, lblDone
          );
        }else{
          windowCodeRangeTest(
              p, OP_Ge, p->start.csr, regCountdown, p->current.csr, lblDone
          );
        }
      }else{
        windowCodeRangeTest(
            p, OP_Gt, p->end.csr, regCountdown, p->current.csr, lblDone
        );
      }
    }else{
      sqlite3VdbeAddOp3(v, OP_IfPos, regCountdown, lblDone, 1);
      VdbeCoverage(v);
    }
  }

  if( op==WINDOW_RETURN_ROW && pMWin->regStartRowid==0 ){
    windowAggFinal(p, 0);
  }
  addrContinue = sqlite3VdbeCurrentAddr(v);

  /* If this is a (RANGE BETWEEN a FOLLOWING AND b FOLLOWING) or
  ** (RANGE BETWEEN b PRECEDING AND a PRECEDING) frame, ensure the 
  ** start cursor does not advance past the end cursor within the 
  ** temporary table. It otherwise might, if (a>b).  */
  if( pMWin->eStart==pMWin->eEnd && regCountdown
   && pMWin->eFrmType==TK_RANGE && op==WINDOW_AGGINVERSE
  ){
    int regRowid1 = sqlite3GetTempReg(pParse);
    int regRowid2 = sqlite3GetTempReg(pParse);
    sqlite3VdbeAddOp2(v, OP_Rowid, p->start.csr, regRowid1);
    sqlite3VdbeAddOp2(v, OP_Rowid, p->end.csr, regRowid2);
    sqlite3VdbeAddOp3(v, OP_Ge, regRowid2, lblDone, regRowid1);
    VdbeCoverage(v);
    sqlite3ReleaseTempReg(pParse, regRowid1);
    sqlite3ReleaseTempReg(pParse, regRowid2);
    assert( pMWin->eStart==TK_PRECEDING || pMWin->eStart==TK_FOLLOWING );
  }

  switch( op ){
    case WINDOW_RETURN_ROW:
      csr = p->current.csr;
      reg = p->current.reg;
      windowReturnOneRow(p);
      break;

    case WINDOW_AGGINVERSE:
      csr = p->start.csr;
      reg = p->start.reg;
      if( pMWin->regStartRowid ){
        assert( pMWin->regEndRowid );
        sqlite3VdbeAddOp2(v, OP_AddImm, pMWin->regStartRowid, 1);
      }else{
        windowAggStep(p, pMWin, csr, 1, p->regArg);
      }
      break;

    default:
      assert( op==WINDOW_AGGSTEP );
      csr = p->end.csr;
      reg = p->end.reg;
      if( pMWin->regStartRowid ){
        assert( pMWin->regEndRowid );
        sqlite3VdbeAddOp2(v, OP_AddImm, pMWin->regEndRowid, 1);
      }else{
        windowAggStep(p, pMWin, csr, 0, p->regArg);
      }
      break;
  }

  if( op==p->eDelete ){
    sqlite3VdbeAddOp1(v, OP_Delete, csr);
    sqlite3VdbeChangeP5(v, OPFLAG_SAVEPOSITION);
  }

  if( jumpOnEof ){
    sqlite3VdbeAddOp2(v, OP_Next, csr, sqlite3VdbeCurrentAddr(v)+2);
    VdbeCoverage(v);
    ret = sqlite3VdbeAddOp0(v, OP_Goto);
  }else{
    sqlite3VdbeAddOp2(v, OP_Next, csr, sqlite3VdbeCurrentAddr(v)+1+bPeer);
    VdbeCoverage(v);
    if( bPeer ){
      sqlite3VdbeAddOp2(v, OP_Goto, 0, lblDone);
    }
  }

  if( bPeer ){
    int nReg = (pMWin->pOrderBy ? pMWin->pOrderBy->nExpr : 0);
    int regTmp = (nReg ? sqlite3GetTempRange(pParse, nReg) : 0);
    windowReadPeerValues(p, csr, regTmp);
    windowIfNewPeer(pParse, pMWin->pOrderBy, regTmp, reg, addrContinue);
    sqlite3ReleaseTempRange(pParse, regTmp, nReg);
  }

  if( addrNextRange ){
    sqlite3VdbeAddOp2(v, OP_Goto, 0, addrNextRange);
  }
  sqlite3VdbeResolveLabel(v, lblDone);
  return ret;
}