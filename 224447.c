void sqlite3VdbeResetStepResult(Vdbe *p){
  p->rc = SQLITE_OK;
}