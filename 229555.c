UnicodeString::removeRef() {
  return umtx_atomic_dec((u_atomic_int32_t *)fUnion.fFields.fArray - 1);
}