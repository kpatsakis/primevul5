void sqlite3WindowFunctions(void){
  static FuncDef aWindowFuncs[] = {
    WINDOWFUNCX(row_number, 0, 0),
    WINDOWFUNCX(dense_rank, 0, 0),
    WINDOWFUNCX(rank, 0, 0),
    WINDOWFUNCALL(percent_rank, 0, 0),
    WINDOWFUNCALL(cume_dist, 0, 0),
    WINDOWFUNCALL(ntile, 1, 0),
    WINDOWFUNCALL(last_value, 1, 0),
    WINDOWFUNCALL(nth_value, 2, 0),
    WINDOWFUNCALL(first_value, 1, 0),
    WINDOWFUNCNOOP(lead, 1, 0),
    WINDOWFUNCNOOP(lead, 2, 0),
    WINDOWFUNCNOOP(lead, 3, 0),
    WINDOWFUNCNOOP(lag, 1, 0),
    WINDOWFUNCNOOP(lag, 2, 0),
    WINDOWFUNCNOOP(lag, 3, 0),
  };
  sqlite3InsertBuiltinFuncs(aWindowFuncs, ArraySize(aWindowFuncs));
}