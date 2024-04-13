UnicodeString::releaseBuffer(int32_t newLength) {
  if(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    if(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      const UChar *array=getArrayStart(), *p=array, *limit=array+capacity;
      while(p<limit && *p!=0) {
        ++p;
      }
      newLength=(int32_t)(p-array);
    } else if(newLength>capacity) {
      newLength=capacity;
    }
    setLength(newLength);
    fUnion.fFields.fLengthAndFlags&=~kOpenGetBuffer;
  }
}