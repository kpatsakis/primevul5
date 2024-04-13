void sqlite3VdbeAssertAbortable(Vdbe *p){
  assert( p->nWrite==0 || p->usesStmtJournal );
}