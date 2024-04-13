static u32 serialGet(
  const unsigned char *buf,     /* Buffer to deserialize from */
  u32 serial_type,              /* Serial type to deserialize */
  Mem *pMem                     /* Memory cell to write value into */
){
  u64 x = FOUR_BYTE_UINT(buf);
  u32 y = FOUR_BYTE_UINT(buf+4);
  x = (x<<32) + y;
  if( serial_type==6 ){
    /* EVIDENCE-OF: R-29851-52272 Value is a big-endian 64-bit
    ** twos-complement integer. */
    pMem->u.i = *(i64*)&x;
    pMem->flags = MEM_Int;
    testcase( pMem->u.i<0 );
  }else{
    /* EVIDENCE-OF: R-57343-49114 Value is a big-endian IEEE 754-2008 64-bit
    ** floating point number. */
#if !defined(NDEBUG) && !defined(SQLITE_OMIT_FLOATING_POINT)
    /* Verify that integers and floating point values use the same
    ** byte order.  Or, that if SQLITE_MIXED_ENDIAN_64BIT_FLOAT is
    ** defined that 64-bit floating point values really are mixed
    ** endian.
    */
    static const u64 t1 = ((u64)0x3ff00000)<<32;
    static const double r1 = 1.0;
    u64 t2 = t1;
    swapMixedEndianFloat(t2);
    assert( sizeof(r1)==sizeof(t2) && memcmp(&r1, &t2, sizeof(r1))==0 );
#endif
    assert( sizeof(x)==8 && sizeof(pMem->u.r)==8 );
    swapMixedEndianFloat(x);
    memcpy(&pMem->u.r, &x, sizeof(x));
    pMem->flags = IsNaN(x) ? MEM_Null : MEM_Real;
  }
  return 8;
}