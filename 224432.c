static void vdbeAssertFieldCountWithinLimits(
  int nKey, const void *pKey,   /* The record to verify */ 
  const KeyInfo *pKeyInfo       /* Compare size with this KeyInfo */
){
  int nField = 0;
  u32 szHdr;
  u32 idx;
  u32 notUsed;
  const unsigned char *aKey = (const unsigned char*)pKey;

  if( CORRUPT_DB ) return;
  idx = getVarint32(aKey, szHdr);
  assert( nKey>=0 );
  assert( szHdr<=(u32)nKey );
  while( idx<szHdr ){
    idx += getVarint32(aKey+idx, notUsed);
    nField++;
  }
  assert( nField <= pKeyInfo->nAllField );
}