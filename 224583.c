RecordCompare sqlite3VdbeFindCompare(UnpackedRecord *p){
  /* varintRecordCompareInt() and varintRecordCompareString() both assume
  ** that the size-of-header varint that occurs at the start of each record
  ** fits in a single byte (i.e. is 127 or less). varintRecordCompareInt()
  ** also assumes that it is safe to overread a buffer by at least the 
  ** maximum possible legal header size plus 8 bytes. Because there is
  ** guaranteed to be at least 74 (but not 136) bytes of padding following each
  ** buffer passed to varintRecordCompareInt() this makes it convenient to
  ** limit the size of the header to 64 bytes in cases where the first field
  ** is an integer.
  **
  ** The easiest way to enforce this limit is to consider only records with
  ** 13 fields or less. If the first field is an integer, the maximum legal
  ** header size is (12*5 + 1 + 1) bytes.  */
  if( p->pKeyInfo->nAllField<=13 ){
    int flags = p->aMem[0].flags;
    if( p->pKeyInfo->aSortFlags[0] ){
      if( p->pKeyInfo->aSortFlags[0] & KEYINFO_ORDER_BIGNULL ){
        return sqlite3VdbeRecordCompare;
      }
      p->r1 = 1;
      p->r2 = -1;
    }else{
      p->r1 = -1;
      p->r2 = 1;
    }
    if( (flags & MEM_Int) ){
      return vdbeRecordCompareInt;
    }
    testcase( flags & MEM_Real );
    testcase( flags & MEM_Null );
    testcase( flags & MEM_Blob );
    if( (flags & (MEM_Real|MEM_IntReal|MEM_Null|MEM_Blob))==0
     && p->pKeyInfo->aColl[0]==0
    ){
      assert( flags & MEM_Str );
      return vdbeRecordCompareString;
    }
  }

  return sqlite3VdbeRecordCompare;
}