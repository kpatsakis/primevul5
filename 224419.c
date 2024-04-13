int sqlite3VdbeReset(Vdbe *p){
#if defined(SQLITE_DEBUG) || defined(VDBE_PROFILE)
  int i;
#endif

  sqlite3 *db;
  db = p->db;

  /* If the VM did not run to completion or if it encountered an
  ** error, then it might not have been halted properly.  So halt
  ** it now.
  */
  sqlite3VdbeHalt(p);

  /* If the VDBE has been run even partially, then transfer the error code
  ** and error message from the VDBE into the main database structure.  But
  ** if the VDBE has just been set to run but has not actually executed any
  ** instructions yet, leave the main database error information unchanged.
  */
  if( p->pc>=0 ){
    vdbeInvokeSqllog(p);
    sqlite3VdbeTransferError(p);
    if( p->runOnlyOnce ) p->expired = 1;
  }else if( p->rc && p->expired ){
    /* The expired flag was set on the VDBE before the first call
    ** to sqlite3_step(). For consistency (since sqlite3_step() was
    ** called), set the database error in this case as well.
    */
    sqlite3ErrorWithMsg(db, p->rc, p->zErrMsg ? "%s" : 0, p->zErrMsg);
  }

  /* Reset register contents and reclaim error message memory.
  */
#ifdef SQLITE_DEBUG
  /* Execute assert() statements to ensure that the Vdbe.apCsr[] and 
  ** Vdbe.aMem[] arrays have already been cleaned up.  */
  if( p->apCsr ) for(i=0; i<p->nCursor; i++) assert( p->apCsr[i]==0 );
  if( p->aMem ){
    for(i=0; i<p->nMem; i++) assert( p->aMem[i].flags==MEM_Undefined );
  }
#endif
  sqlite3DbFree(db, p->zErrMsg);
  p->zErrMsg = 0;
  p->pResultSet = 0;
#ifdef SQLITE_DEBUG
  p->nWrite = 0;
#endif

  /* Save profiling information from this VDBE run.
  */
#ifdef VDBE_PROFILE
  {
    FILE *out = fopen("vdbe_profile.out", "a");
    if( out ){
      fprintf(out, "---- ");
      for(i=0; i<p->nOp; i++){
        fprintf(out, "%02x", p->aOp[i].opcode);
      }
      fprintf(out, "\n");
      if( p->zSql ){
        char c, pc = 0;
        fprintf(out, "-- ");
        for(i=0; (c = p->zSql[i])!=0; i++){
          if( pc=='\n' ) fprintf(out, "-- ");
          putc(c, out);
          pc = c;
        }
        if( pc!='\n' ) fprintf(out, "\n");
      }
      for(i=0; i<p->nOp; i++){
        char zHdr[100];
        sqlite3_snprintf(sizeof(zHdr), zHdr, "%6u %12llu %8llu ",
           p->aOp[i].cnt,
           p->aOp[i].cycles,
           p->aOp[i].cnt>0 ? p->aOp[i].cycles/p->aOp[i].cnt : 0
        );
        fprintf(out, "%s", zHdr);
        sqlite3VdbePrintOp(out, i, &p->aOp[i]);
      }
      fclose(out);
    }
  }
#endif
  p->magic = VDBE_MAGIC_RESET;
  return p->rc & db->errMask;
}