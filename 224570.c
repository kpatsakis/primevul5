void sqlite3VdbeRunOnlyOnce(Vdbe *p){
  p->runOnlyOnce = 1;
}