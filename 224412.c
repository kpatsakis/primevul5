void sqlite3VdbeChangeOpcode(Vdbe *p, int addr, u8 iNewOpcode){
  sqlite3VdbeGetOp(p,addr)->opcode = iNewOpcode;
}