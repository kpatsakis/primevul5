void sqlite3VdbeExplain(Parse *pParse, u8 bPush, const char *zFmt, ...){
#ifndef SQLITE_DEBUG
  /* Always include the OP_Explain opcodes if SQLITE_DEBUG is defined.
  ** But omit them (for performance) during production builds */
  if( pParse->explain==2 )
#endif
  {
    char *zMsg;
    Vdbe *v;
    va_list ap;
    int iThis;
    va_start(ap, zFmt);
    zMsg = sqlite3VMPrintf(pParse->db, zFmt, ap);
    va_end(ap);
    v = pParse->pVdbe;
    iThis = v->nOp;
    sqlite3VdbeAddOp4(v, OP_Explain, iThis, pParse->addrExplain, 0,
                      zMsg, P4_DYNAMIC);
    sqlite3ExplainBreakpoint(bPush?"PUSH":"", sqlite3VdbeGetOp(v,-1)->p4.z);
    if( bPush){
      pParse->addrExplain = iThis;
    }
  }
}