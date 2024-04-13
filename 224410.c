int sqlite3VdbeAddFunctionCall(
  Parse *pParse,        /* Parsing context */
  int p1,               /* Constant argument mask */
  int p2,               /* First argument register */
  int p3,               /* Register into which results are written */
  int nArg,             /* Number of argument */
  const FuncDef *pFunc, /* The function to be invoked */
  int eCallCtx          /* Calling context */
){
  Vdbe *v = pParse->pVdbe;
  int nByte;
  int addr;
  sqlite3_context *pCtx;
  assert( v );
  nByte = sizeof(*pCtx) + (nArg-1)*sizeof(sqlite3_value*);
  pCtx = sqlite3DbMallocRawNN(pParse->db, nByte);
  if( pCtx==0 ){
    assert( pParse->db->mallocFailed );
    freeEphemeralFunction(pParse->db, (FuncDef*)pFunc);
    return 0;
  }
  pCtx->pOut = 0;
  pCtx->pFunc = (FuncDef*)pFunc;
  pCtx->pVdbe = 0;
  pCtx->isError = 0;
  pCtx->argc = nArg;
  pCtx->iOp = sqlite3VdbeCurrentAddr(v);
  addr = sqlite3VdbeAddOp4(v, eCallCtx ? OP_PureFunc : OP_Function,
                           p1, p2, p3, (char*)pCtx, P4_FUNCCTX);
  sqlite3VdbeChangeP5(v, eCallCtx & NC_SelfRef);
  return addr;
}