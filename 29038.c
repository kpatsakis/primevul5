GfxShadingBitBuf::GfxShadingBitBuf(Stream *strA) {
  str = strA;
  str->reset();
  bitBuf = 0;
  nBits = 0;
}
