void sqlite3VdbeVerifyAbortable(Vdbe *p, int onError){
  if( onError==OE_Abort ) sqlite3VdbeAddOp0(p, OP_Abortable);
}