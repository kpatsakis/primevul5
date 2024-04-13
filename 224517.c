static void freeP4(sqlite3 *db, int p4type, void *p4){
  assert( db );
  switch( p4type ){
    case P4_FUNCCTX: {
      freeP4FuncCtx(db, (sqlite3_context*)p4);
      break;
    }
    case P4_REAL:
    case P4_INT64:
    case P4_DYNAMIC:
    case P4_DYNBLOB:
    case P4_INTARRAY: {
      sqlite3DbFree(db, p4);
      break;
    }
    case P4_KEYINFO: {
      if( db->pnBytesFreed==0 ) sqlite3KeyInfoUnref((KeyInfo*)p4);
      break;
    }
#ifdef SQLITE_ENABLE_CURSOR_HINTS
    case P4_EXPR: {
      sqlite3ExprDelete(db, (Expr*)p4);
      break;
    }
#endif
    case P4_FUNCDEF: {
      freeEphemeralFunction(db, (FuncDef*)p4);
      break;
    }
    case P4_MEM: {
      if( db->pnBytesFreed==0 ){
        sqlite3ValueFree((sqlite3_value*)p4);
      }else{
        freeP4Mem(db, (Mem*)p4);
      }
      break;
    }
    case P4_VTAB : {
      if( db->pnBytesFreed==0 ) sqlite3VtabUnlock((VTable *)p4);
      break;
    }
  }
}