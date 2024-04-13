int sqlite3VdbeHasSubProgram(Vdbe *pVdbe){
  return pVdbe->pProgram!=0;
}