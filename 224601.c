static void initMemArray(Mem *p, int N, sqlite3 *db, u16 flags){
  while( (N--)>0 ){
    p->db = db;
    p->flags = flags;
    p->szMalloc = 0;
#ifdef SQLITE_DEBUG
    p->pScopyFrom = 0;
#endif
    p++;
  }
}