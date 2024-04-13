static int vdbeCompareMemString(
  const Mem *pMem1,
  const Mem *pMem2,
  const CollSeq *pColl,
  u8 *prcErr                      /* If an OOM occurs, set to SQLITE_NOMEM */
){
  if( pMem1->enc==pColl->enc ){
    /* The strings are already in the correct encoding.  Call the
     ** comparison function directly */
    return pColl->xCmp(pColl->pUser,pMem1->n,pMem1->z,pMem2->n,pMem2->z);
  }else{
    int rc;
    const void *v1, *v2;
    Mem c1;
    Mem c2;
    sqlite3VdbeMemInit(&c1, pMem1->db, MEM_Null);
    sqlite3VdbeMemInit(&c2, pMem1->db, MEM_Null);
    sqlite3VdbeMemShallowCopy(&c1, pMem1, MEM_Ephem);
    sqlite3VdbeMemShallowCopy(&c2, pMem2, MEM_Ephem);
    v1 = sqlite3ValueText((sqlite3_value*)&c1, pColl->enc);
    v2 = sqlite3ValueText((sqlite3_value*)&c2, pColl->enc);
    if( (v1==0 || v2==0) ){
      if( prcErr ) *prcErr = SQLITE_NOMEM_BKPT;
      rc = 0;
    }else{
      rc = pColl->xCmp(pColl->pUser, c1.n, v1, c2.n, v2);
    }
    sqlite3VdbeMemRelease(&c1);
    sqlite3VdbeMemRelease(&c2);
    return rc;
  }
}