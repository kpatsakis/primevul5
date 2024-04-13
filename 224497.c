void sqlite3VdbeSetLineNumber(Vdbe *v, int iLine){
  sqlite3VdbeGetOp(v,-1)->iSrcLine = iLine;
}