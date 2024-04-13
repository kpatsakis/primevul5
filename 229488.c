UnicodeString::refCount() const {
  return umtx_loadAcquire(*((u_atomic_int32_t *)fUnion.fFields.fArray - 1));
}