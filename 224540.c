int sqlite3VdbeAssertMayAbort(Vdbe *v, int mayAbort){
  int hasAbort = 0;
  int hasFkCounter = 0;
  int hasCreateTable = 0;
  int hasCreateIndex = 0;
  int hasInitCoroutine = 0;
  Op *pOp;
  VdbeOpIter sIter;
  memset(&sIter, 0, sizeof(sIter));
  sIter.v = v;

  while( (pOp = opIterNext(&sIter))!=0 ){
    int opcode = pOp->opcode;
    if( opcode==OP_Destroy || opcode==OP_VUpdate || opcode==OP_VRename 
     || opcode==OP_VDestroy
     || opcode==OP_VCreate
     || (opcode==OP_ParseSchema && pOp->p4.z==0)
     || ((opcode==OP_Halt || opcode==OP_HaltIfNull) 
      && ((pOp->p1)!=SQLITE_OK && pOp->p2==OE_Abort))
    ){
      hasAbort = 1;
      break;
    }
    if( opcode==OP_CreateBtree && pOp->p3==BTREE_INTKEY ) hasCreateTable = 1;
    if( mayAbort ){
      /* hasCreateIndex may also be set for some DELETE statements that use
      ** OP_Clear. So this routine may end up returning true in the case 
      ** where a "DELETE FROM tbl" has a statement-journal but does not
      ** require one. This is not so bad - it is an inefficiency, not a bug. */
      if( opcode==OP_CreateBtree && pOp->p3==BTREE_BLOBKEY ) hasCreateIndex = 1;
      if( opcode==OP_Clear ) hasCreateIndex = 1;
    }
    if( opcode==OP_InitCoroutine ) hasInitCoroutine = 1;
#ifndef SQLITE_OMIT_FOREIGN_KEY
    if( opcode==OP_FkCounter && pOp->p1==0 && pOp->p2==1 ){
      hasFkCounter = 1;
    }
#endif
  }
  sqlite3DbFree(v->db, sIter.apSub);

  /* Return true if hasAbort==mayAbort. Or if a malloc failure occurred.
  ** If malloc failed, then the while() loop above may not have iterated
  ** through all opcodes and hasAbort may be set incorrectly. Return
  ** true for this case to prevent the assert() in the callers frame
  ** from failing.  */
  return ( v->db->mallocFailed || hasAbort==mayAbort || hasFkCounter
        || (hasCreateTable && hasInitCoroutine) || hasCreateIndex
  );
}