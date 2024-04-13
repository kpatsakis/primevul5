int sqlite3VdbeList(
  Vdbe *p                   /* The VDBE */
){
  int nRow;                            /* Stop when row count reaches this */
  int nSub = 0;                        /* Number of sub-vdbes seen so far */
  SubProgram **apSub = 0;              /* Array of sub-vdbes */
  Mem *pSub = 0;                       /* Memory cell hold array of subprogs */
  sqlite3 *db = p->db;                 /* The database connection */
  int i;                               /* Loop counter */
  int rc = SQLITE_OK;                  /* Return code */
  Mem *pMem = &p->aMem[1];             /* First Mem of result set */
  int bListSubprogs = (p->explain==1 || (db->flags & SQLITE_TriggerEQP)!=0);
  Op *pOp = 0;

  assert( p->explain );
  assert( p->magic==VDBE_MAGIC_RUN );
  assert( p->rc==SQLITE_OK || p->rc==SQLITE_BUSY || p->rc==SQLITE_NOMEM );

  /* Even though this opcode does not use dynamic strings for
  ** the result, result columns may become dynamic if the user calls
  ** sqlite3_column_text16(), causing a translation to UTF-16 encoding.
  */
  releaseMemArray(pMem, 8);
  p->pResultSet = 0;

  if( p->rc==SQLITE_NOMEM ){
    /* This happens if a malloc() inside a call to sqlite3_column_text() or
    ** sqlite3_column_text16() failed.  */
    sqlite3OomFault(db);
    return SQLITE_ERROR;
  }

  /* When the number of output rows reaches nRow, that means the
  ** listing has finished and sqlite3_step() should return SQLITE_DONE.
  ** nRow is the sum of the number of rows in the main program, plus
  ** the sum of the number of rows in all trigger subprograms encountered
  ** so far.  The nRow value will increase as new trigger subprograms are
  ** encountered, but p->pc will eventually catch up to nRow.
  */
  nRow = p->nOp;
  if( bListSubprogs ){
    /* The first 8 memory cells are used for the result set.  So we will
    ** commandeer the 9th cell to use as storage for an array of pointers
    ** to trigger subprograms.  The VDBE is guaranteed to have at least 9
    ** cells.  */
    assert( p->nMem>9 );
    pSub = &p->aMem[9];
    if( pSub->flags&MEM_Blob ){
      /* On the first call to sqlite3_step(), pSub will hold a NULL.  It is
      ** initialized to a BLOB by the P4_SUBPROGRAM processing logic below */
      nSub = pSub->n/sizeof(Vdbe*);
      apSub = (SubProgram **)pSub->z;
    }
    for(i=0; i<nSub; i++){
      nRow += apSub[i]->nOp;
    }
  }

  while(1){  /* Loop exits via break */
    i = p->pc++;
    if( i>=nRow ){
      p->rc = SQLITE_OK;
      rc = SQLITE_DONE;
      break;
    }
    if( i<p->nOp ){
      /* The output line number is small enough that we are still in the
      ** main program. */
      pOp = &p->aOp[i];
    }else{
      /* We are currently listing subprograms.  Figure out which one and
      ** pick up the appropriate opcode. */
      int j;
      i -= p->nOp;
      assert( apSub!=0 );
      assert( nSub>0 );
      for(j=0; i>=apSub[j]->nOp; j++){
        i -= apSub[j]->nOp;
        assert( i<apSub[j]->nOp || j+1<nSub );
      }
      pOp = &apSub[j]->aOp[i];
    }

    /* When an OP_Program opcode is encounter (the only opcode that has
    ** a P4_SUBPROGRAM argument), expand the size of the array of subprograms
    ** kept in p->aMem[9].z to hold the new program - assuming this subprogram
    ** has not already been seen.
    */
    if( bListSubprogs && pOp->p4type==P4_SUBPROGRAM ){
      int nByte = (nSub+1)*sizeof(SubProgram*);
      int j;
      for(j=0; j<nSub; j++){
        if( apSub[j]==pOp->p4.pProgram ) break;
      }
      if( j==nSub ){
        p->rc = sqlite3VdbeMemGrow(pSub, nByte, nSub!=0);
        if( p->rc!=SQLITE_OK ){
          rc = SQLITE_ERROR;
          break;
        }
        apSub = (SubProgram **)pSub->z;
        apSub[nSub++] = pOp->p4.pProgram;
        pSub->flags |= MEM_Blob;
        pSub->n = nSub*sizeof(SubProgram*);
        nRow += pOp->p4.pProgram->nOp;
      }
    }
    if( p->explain<2 ) break;
    if( pOp->opcode==OP_Explain ) break;
    if( pOp->opcode==OP_Init && p->pc>1 ) break;
  }

  if( rc==SQLITE_OK ){
    if( db->u1.isInterrupted ){
      p->rc = SQLITE_INTERRUPT;
      rc = SQLITE_ERROR;
      sqlite3VdbeError(p, sqlite3ErrStr(p->rc));
    }else{
      char *zP4;
      if( p->explain==1 ){
        pMem->flags = MEM_Int;
        pMem->u.i = i;                                /* Program counter */
        pMem++;
    
        pMem->flags = MEM_Static|MEM_Str|MEM_Term;
        pMem->z = (char*)sqlite3OpcodeName(pOp->opcode); /* Opcode */
        assert( pMem->z!=0 );
        pMem->n = sqlite3Strlen30(pMem->z);
        pMem->enc = SQLITE_UTF8;
        pMem++;
      }

      pMem->flags = MEM_Int;
      pMem->u.i = pOp->p1;                          /* P1 */
      pMem++;

      pMem->flags = MEM_Int;
      pMem->u.i = pOp->p2;                          /* P2 */
      pMem++;

      pMem->flags = MEM_Int;
      pMem->u.i = pOp->p3;                          /* P3 */
      pMem++;

      if( sqlite3VdbeMemClearAndResize(pMem, 100) ){ /* P4 */
        assert( p->db->mallocFailed );
        return SQLITE_ERROR;
      }
      pMem->flags = MEM_Str|MEM_Term;
      zP4 = displayP4(pOp, pMem->z, pMem->szMalloc);
      if( zP4!=pMem->z ){
        pMem->n = 0;
        sqlite3VdbeMemSetStr(pMem, zP4, -1, SQLITE_UTF8, 0);
      }else{
        assert( pMem->z!=0 );
        pMem->n = sqlite3Strlen30(pMem->z);
        pMem->enc = SQLITE_UTF8;
      }
      pMem++;

      if( p->explain==1 ){
        if( sqlite3VdbeMemClearAndResize(pMem, 4) ){
          assert( p->db->mallocFailed );
          return SQLITE_ERROR;
        }
        pMem->flags = MEM_Str|MEM_Term;
        pMem->n = 2;
        sqlite3_snprintf(3, pMem->z, "%.2x", pOp->p5);   /* P5 */
        pMem->enc = SQLITE_UTF8;
        pMem++;
    
#ifdef SQLITE_ENABLE_EXPLAIN_COMMENTS
        if( sqlite3VdbeMemClearAndResize(pMem, 500) ){
          assert( p->db->mallocFailed );
          return SQLITE_ERROR;
        }
        pMem->flags = MEM_Str|MEM_Term;
        pMem->n = displayComment(pOp, zP4, pMem->z, 500);
        pMem->enc = SQLITE_UTF8;
#else
        pMem->flags = MEM_Null;                       /* Comment */
#endif
      }

      p->nResColumn = 8 - 4*(p->explain-1);
      p->pResultSet = &p->aMem[1];
      p->rc = SQLITE_OK;
      rc = SQLITE_ROW;
    }
  }
  return rc;
}