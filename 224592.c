static void windowCheckValue(Parse *pParse, int reg, int eCond){
  static const char *azErr[] = {
    "frame starting offset must be a non-negative integer",
    "frame ending offset must be a non-negative integer",
    "second argument to nth_value must be a positive integer",
    "frame starting offset must be a non-negative number",
    "frame ending offset must be a non-negative number",
  };
  static int aOp[] = { OP_Ge, OP_Ge, OP_Gt, OP_Ge, OP_Ge };
  Vdbe *v = sqlite3GetVdbe(pParse);
  int regZero = sqlite3GetTempReg(pParse);
  assert( eCond>=0 && eCond<ArraySize(azErr) );
  sqlite3VdbeAddOp2(v, OP_Integer, 0, regZero);
  if( eCond>=WINDOW_STARTING_NUM ){
    int regString = sqlite3GetTempReg(pParse);
    sqlite3VdbeAddOp4(v, OP_String8, 0, regString, 0, "", P4_STATIC);
    sqlite3VdbeAddOp3(v, OP_Ge, regString, sqlite3VdbeCurrentAddr(v)+2, reg);
    sqlite3VdbeChangeP5(v, SQLITE_AFF_NUMERIC|SQLITE_JUMPIFNULL);
    VdbeCoverage(v);
    assert( eCond==3 || eCond==4 );
    VdbeCoverageIf(v, eCond==3);
    VdbeCoverageIf(v, eCond==4);
  }else{
    sqlite3VdbeAddOp2(v, OP_MustBeInt, reg, sqlite3VdbeCurrentAddr(v)+2);
    VdbeCoverage(v);
    assert( eCond==0 || eCond==1 || eCond==2 );
    VdbeCoverageIf(v, eCond==0);
    VdbeCoverageIf(v, eCond==1);
    VdbeCoverageIf(v, eCond==2);
  }
  sqlite3VdbeAddOp3(v, aOp[eCond], regZero, sqlite3VdbeCurrentAddr(v)+2, reg);
  VdbeCoverageNeverNullIf(v, eCond==0); /* NULL case captured by */
  VdbeCoverageNeverNullIf(v, eCond==1); /*   the OP_MustBeInt */
  VdbeCoverageNeverNullIf(v, eCond==2);
  VdbeCoverageNeverNullIf(v, eCond==3); /* NULL case caught by */
  VdbeCoverageNeverNullIf(v, eCond==4); /*   the OP_Ge */
  sqlite3MayAbort(pParse);
  sqlite3VdbeAddOp2(v, OP_Halt, SQLITE_ERROR, OE_Abort);
  sqlite3VdbeAppendP4(v, (void*)azErr[eCond], P4_STATIC);
  sqlite3ReleaseTempReg(pParse, regZero);
}