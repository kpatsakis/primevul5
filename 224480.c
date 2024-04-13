SQLITE_NOINLINE int sqlite3BlobCompare(const Mem *pB1, const Mem *pB2){
  int c;
  int n1 = pB1->n;
  int n2 = pB2->n;

  /* It is possible to have a Blob value that has some non-zero content
  ** followed by zero content.  But that only comes up for Blobs formed
  ** by the OP_MakeRecord opcode, and such Blobs never get passed into
  ** sqlite3MemCompare(). */
  assert( (pB1->flags & MEM_Zero)==0 || n1==0 );
  assert( (pB2->flags & MEM_Zero)==0 || n2==0 );

  if( (pB1->flags|pB2->flags) & MEM_Zero ){
    if( pB1->flags & pB2->flags & MEM_Zero ){
      return pB1->u.nZero - pB2->u.nZero;
    }else if( pB1->flags & MEM_Zero ){
      if( !isAllZero(pB2->z, pB2->n) ) return -1;
      return pB1->u.nZero - n2;
    }else{
      if( !isAllZero(pB1->z, pB1->n) ) return +1;
      return n1 - pB2->u.nZero;
    }
  }
  c = memcmp(pB1->z, pB2->z, n1>n2 ? n2 : n1);
  if( c ) return c;
  return n1 - n2;
}