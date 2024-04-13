int sqlite3NotPureFunc(sqlite3_context *pCtx){
  const VdbeOp *pOp;
#ifdef SQLITE_ENABLE_STAT4
  if( pCtx->pVdbe==0 ) return 1;
#endif
  pOp = pCtx->pVdbe->aOp + pCtx->iOp;
  if( pOp->opcode==OP_PureFunc ){
    const char *zContext;
    char *zMsg;
    if( pOp->p5 & NC_IsCheck ){
      zContext = "a CHECK constraint";
    }else if( pOp->p5 & NC_GenCol ){
      zContext = "a generated column";
    }else{
      zContext = "an index";
    }
    zMsg = sqlite3_mprintf("non-deterministic use of %s() in %s",
                           pCtx->pFunc->zName, zContext);
    sqlite3_result_error(pCtx, zMsg, -1);
    sqlite3_free(zMsg);
    return 0;
  }
  return 1;
}