UnicodeString::addRef() {
  umtx_atomic_inc((u_atomic_int32_t *)fUnion.fFields.fArray - 1);
}