static char *displayP4(Op *pOp, char *zTemp, int nTemp){
  char *zP4 = zTemp;
  StrAccum x;
  assert( nTemp>=20 );
  sqlite3StrAccumInit(&x, 0, zTemp, nTemp, 0);
  switch( pOp->p4type ){
    case P4_KEYINFO: {
      int j;
      KeyInfo *pKeyInfo = pOp->p4.pKeyInfo;
      assert( pKeyInfo->aSortFlags!=0 );
      sqlite3_str_appendf(&x, "k(%d", pKeyInfo->nKeyField);
      for(j=0; j<pKeyInfo->nKeyField; j++){
        CollSeq *pColl = pKeyInfo->aColl[j];
        const char *zColl = pColl ? pColl->zName : "";
        if( strcmp(zColl, "BINARY")==0 ) zColl = "B";
        sqlite3_str_appendf(&x, ",%s%s%s", 
               (pKeyInfo->aSortFlags[j] & KEYINFO_ORDER_DESC) ? "-" : "", 
               (pKeyInfo->aSortFlags[j] & KEYINFO_ORDER_BIGNULL)? "N." : "", 
               zColl);
      }
      sqlite3_str_append(&x, ")", 1);
      break;
    }
#ifdef SQLITE_ENABLE_CURSOR_HINTS
    case P4_EXPR: {
      displayP4Expr(&x, pOp->p4.pExpr);
      break;
    }
#endif
    case P4_COLLSEQ: {
      CollSeq *pColl = pOp->p4.pColl;
      sqlite3_str_appendf(&x, "(%.20s)", pColl->zName);
      break;
    }
    case P4_FUNCDEF: {
      FuncDef *pDef = pOp->p4.pFunc;
      sqlite3_str_appendf(&x, "%s(%d)", pDef->zName, pDef->nArg);
      break;
    }
    case P4_FUNCCTX: {
      FuncDef *pDef = pOp->p4.pCtx->pFunc;
      sqlite3_str_appendf(&x, "%s(%d)", pDef->zName, pDef->nArg);
      break;
    }
    case P4_INT64: {
      sqlite3_str_appendf(&x, "%lld", *pOp->p4.pI64);
      break;
    }
    case P4_INT32: {
      sqlite3_str_appendf(&x, "%d", pOp->p4.i);
      break;
    }
    case P4_REAL: {
      sqlite3_str_appendf(&x, "%.16g", *pOp->p4.pReal);
      break;
    }
    case P4_MEM: {
      Mem *pMem = pOp->p4.pMem;
      if( pMem->flags & MEM_Str ){
        zP4 = pMem->z;
      }else if( pMem->flags & (MEM_Int|MEM_IntReal) ){
        sqlite3_str_appendf(&x, "%lld", pMem->u.i);
      }else if( pMem->flags & MEM_Real ){
        sqlite3_str_appendf(&x, "%.16g", pMem->u.r);
      }else if( pMem->flags & MEM_Null ){
        zP4 = "NULL";
      }else{
        assert( pMem->flags & MEM_Blob );
        zP4 = "(blob)";
      }
      break;
    }
#ifndef SQLITE_OMIT_VIRTUALTABLE
    case P4_VTAB: {
      sqlite3_vtab *pVtab = pOp->p4.pVtab->pVtab;
      sqlite3_str_appendf(&x, "vtab:%p", pVtab);
      break;
    }
#endif
    case P4_INTARRAY: {
      int i;
      int *ai = pOp->p4.ai;
      int n = ai[0];   /* The first element of an INTARRAY is always the
                       ** count of the number of elements to follow */
      for(i=1; i<=n; i++){
        sqlite3_str_appendf(&x, ",%d", ai[i]);
      }
      zTemp[0] = '[';
      sqlite3_str_append(&x, "]", 1);
      break;
    }
    case P4_SUBPROGRAM: {
      sqlite3_str_appendf(&x, "program");
      break;
    }
    case P4_DYNBLOB:
    case P4_ADVANCE: {
      zTemp[0] = 0;
      break;
    }
    case P4_TABLE: {
      sqlite3_str_appendf(&x, "%s", pOp->p4.pTab->zName);
      break;
    }
    default: {
      zP4 = pOp->p4.z;
      if( zP4==0 ){
        zP4 = zTemp;
        zTemp[0] = 0;
      }
    }
  }
  sqlite3StrAccumFinish(&x);
  assert( zP4!=0 );
  return zP4;
}