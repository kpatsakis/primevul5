int sqlite3VdbeRecordCompareWithSkip(
  int nKey1, const void *pKey1,   /* Left key */
  UnpackedRecord *pPKey2,         /* Right key */
  int bSkip                       /* If true, skip the first field */
){
  u32 d1;                         /* Offset into aKey[] of next data element */
  int i;                          /* Index of next field to compare */
  u32 szHdr1;                     /* Size of record header in bytes */
  u32 idx1;                       /* Offset of first type in header */
  int rc = 0;                     /* Return value */
  Mem *pRhs = pPKey2->aMem;       /* Next field of pPKey2 to compare */
  KeyInfo *pKeyInfo;
  const unsigned char *aKey1 = (const unsigned char *)pKey1;
  Mem mem1;

  /* If bSkip is true, then the caller has already determined that the first
  ** two elements in the keys are equal. Fix the various stack variables so
  ** that this routine begins comparing at the second field. */
  if( bSkip ){
    u32 s1;
    idx1 = 1 + getVarint32(&aKey1[1], s1);
    szHdr1 = aKey1[0];
    d1 = szHdr1 + sqlite3VdbeSerialTypeLen(s1);
    i = 1;
    pRhs++;
  }else{
    idx1 = getVarint32(aKey1, szHdr1);
    d1 = szHdr1;
    i = 0;
  }
  if( d1>(unsigned)nKey1 ){ 
    pPKey2->errCode = (u8)SQLITE_CORRUPT_BKPT;
    return 0;  /* Corruption */
  }

  VVA_ONLY( mem1.szMalloc = 0; ) /* Only needed by assert() statements */
  assert( pPKey2->pKeyInfo->nAllField>=pPKey2->nField 
       || CORRUPT_DB );
  assert( pPKey2->pKeyInfo->aSortFlags!=0 );
  assert( pPKey2->pKeyInfo->nKeyField>0 );
  assert( idx1<=szHdr1 || CORRUPT_DB );
  do{
    u32 serial_type;

    /* RHS is an integer */
    if( pRhs->flags & (MEM_Int|MEM_IntReal) ){
      testcase( pRhs->flags & MEM_Int );
      testcase( pRhs->flags & MEM_IntReal );
      serial_type = aKey1[idx1];
      testcase( serial_type==12 );
      if( serial_type>=10 ){
        rc = +1;
      }else if( serial_type==0 ){
        rc = -1;
      }else if( serial_type==7 ){
        sqlite3VdbeSerialGet(&aKey1[d1], serial_type, &mem1);
        rc = -sqlite3IntFloatCompare(pRhs->u.i, mem1.u.r);
      }else{
        i64 lhs = vdbeRecordDecodeInt(serial_type, &aKey1[d1]);
        i64 rhs = pRhs->u.i;
        if( lhs<rhs ){
          rc = -1;
        }else if( lhs>rhs ){
          rc = +1;
        }
      }
    }

    /* RHS is real */
    else if( pRhs->flags & MEM_Real ){
      serial_type = aKey1[idx1];
      if( serial_type>=10 ){
        /* Serial types 12 or greater are strings and blobs (greater than
        ** numbers). Types 10 and 11 are currently "reserved for future 
        ** use", so it doesn't really matter what the results of comparing
        ** them to numberic values are.  */
        rc = +1;
      }else if( serial_type==0 ){
        rc = -1;
      }else{
        sqlite3VdbeSerialGet(&aKey1[d1], serial_type, &mem1);
        if( serial_type==7 ){
          if( mem1.u.r<pRhs->u.r ){
            rc = -1;
          }else if( mem1.u.r>pRhs->u.r ){
            rc = +1;
          }
        }else{
          rc = sqlite3IntFloatCompare(mem1.u.i, pRhs->u.r);
        }
      }
    }

    /* RHS is a string */
    else if( pRhs->flags & MEM_Str ){
      getVarint32(&aKey1[idx1], serial_type);
      testcase( serial_type==12 );
      if( serial_type<12 ){
        rc = -1;
      }else if( !(serial_type & 0x01) ){
        rc = +1;
      }else{
        mem1.n = (serial_type - 12) / 2;
        testcase( (d1+mem1.n)==(unsigned)nKey1 );
        testcase( (d1+mem1.n+1)==(unsigned)nKey1 );
        if( (d1+mem1.n) > (unsigned)nKey1
         || (pKeyInfo = pPKey2->pKeyInfo)->nAllField<=i
        ){
          pPKey2->errCode = (u8)SQLITE_CORRUPT_BKPT;
          return 0;                /* Corruption */
        }else if( pKeyInfo->aColl[i] ){
          mem1.enc = pKeyInfo->enc;
          mem1.db = pKeyInfo->db;
          mem1.flags = MEM_Str;
          mem1.z = (char*)&aKey1[d1];
          rc = vdbeCompareMemString(
              &mem1, pRhs, pKeyInfo->aColl[i], &pPKey2->errCode
          );
        }else{
          int nCmp = MIN(mem1.n, pRhs->n);
          rc = memcmp(&aKey1[d1], pRhs->z, nCmp);
          if( rc==0 ) rc = mem1.n - pRhs->n; 
        }
      }
    }

    /* RHS is a blob */
    else if( pRhs->flags & MEM_Blob ){
      assert( (pRhs->flags & MEM_Zero)==0 || pRhs->n==0 );
      getVarint32(&aKey1[idx1], serial_type);
      testcase( serial_type==12 );
      if( serial_type<12 || (serial_type & 0x01) ){
        rc = -1;
      }else{
        int nStr = (serial_type - 12) / 2;
        testcase( (d1+nStr)==(unsigned)nKey1 );
        testcase( (d1+nStr+1)==(unsigned)nKey1 );
        if( (d1+nStr) > (unsigned)nKey1 ){
          pPKey2->errCode = (u8)SQLITE_CORRUPT_BKPT;
          return 0;                /* Corruption */
        }else if( pRhs->flags & MEM_Zero ){
          if( !isAllZero((const char*)&aKey1[d1],nStr) ){
            rc = 1;
          }else{
            rc = nStr - pRhs->u.nZero;
          }
        }else{
          int nCmp = MIN(nStr, pRhs->n);
          rc = memcmp(&aKey1[d1], pRhs->z, nCmp);
          if( rc==0 ) rc = nStr - pRhs->n;
        }
      }
    }

    /* RHS is null */
    else{
      serial_type = aKey1[idx1];
      rc = (serial_type!=0);
    }

    if( rc!=0 ){
      int sortFlags = pPKey2->pKeyInfo->aSortFlags[i];
      if( sortFlags ){
        if( (sortFlags & KEYINFO_ORDER_BIGNULL)==0
         || ((sortFlags & KEYINFO_ORDER_DESC)
           !=(serial_type==0 || (pRhs->flags&MEM_Null)))
        ){
          rc = -rc;
        }
      }
      assert( vdbeRecordCompareDebug(nKey1, pKey1, pPKey2, rc) );
      assert( mem1.szMalloc==0 );  /* See comment below */
      return rc;
    }

    i++;
    if( i==pPKey2->nField ) break;
    pRhs++;
    d1 += sqlite3VdbeSerialTypeLen(serial_type);
    idx1 += sqlite3VarintLen(serial_type);
  }while( idx1<(unsigned)szHdr1 && d1<=(unsigned)nKey1 );

  /* No memory allocation is ever used on mem1.  Prove this using
  ** the following assert().  If the assert() fails, it indicates a
  ** memory leak and a need to call sqlite3VdbeMemRelease(&mem1).  */
  assert( mem1.szMalloc==0 );

  /* rc==0 here means that one or both of the keys ran out of fields and
  ** all the fields up to that point were equal. Return the default_rc
  ** value.  */
  assert( CORRUPT_DB 
       || vdbeRecordCompareDebug(nKey1, pKey1, pPKey2, pPKey2->default_rc) 
       || pPKey2->pKeyInfo->db->mallocFailed
  );
  pPKey2->eqSeen = 1;
  return pPKey2->default_rc;
}