static int vdbeRecordCompareString(
  int nKey1, const void *pKey1, /* Left key */
  UnpackedRecord *pPKey2        /* Right key */
){
  const u8 *aKey1 = (const u8*)pKey1;
  int serial_type;
  int res;

  assert( pPKey2->aMem[0].flags & MEM_Str );
  vdbeAssertFieldCountWithinLimits(nKey1, pKey1, pPKey2->pKeyInfo);
  getVarint32(&aKey1[1], serial_type);
  if( serial_type<12 ){
    res = pPKey2->r1;      /* (pKey1/nKey1) is a number or a null */
  }else if( !(serial_type & 0x01) ){ 
    res = pPKey2->r2;      /* (pKey1/nKey1) is a blob */
  }else{
    int nCmp;
    int nStr;
    int szHdr = aKey1[0];

    nStr = (serial_type-12) / 2;
    if( (szHdr + nStr) > nKey1 ){
      pPKey2->errCode = (u8)SQLITE_CORRUPT_BKPT;
      return 0;    /* Corruption */
    }
    nCmp = MIN( pPKey2->aMem[0].n, nStr );
    res = memcmp(&aKey1[szHdr], pPKey2->aMem[0].z, nCmp);

    if( res>0 ){
      res = pPKey2->r2;
    }else if( res<0 ){
      res = pPKey2->r1;
    }else{
      res = nStr - pPKey2->aMem[0].n;
      if( res==0 ){
        if( pPKey2->nField>1 ){
          res = sqlite3VdbeRecordCompareWithSkip(nKey1, pKey1, pPKey2, 1);
        }else{
          res = pPKey2->default_rc;
          pPKey2->eqSeen = 1;
        }
      }else if( res>0 ){
        res = pPKey2->r2;
      }else{
        res = pPKey2->r1;
      }
    }
  }

  assert( vdbeRecordCompareDebug(nKey1, pKey1, pPKey2, res)
       || CORRUPT_DB
       || pPKey2->pKeyInfo->db->mallocFailed
  );
  return res;
}