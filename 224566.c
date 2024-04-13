void sqlite3VdbeCountChanges(Vdbe *v){
  v->changeCntOn = 1;
}