void sqlite3VdbeReusable(Vdbe *p){
  p->runOnlyOnce = 0;
}