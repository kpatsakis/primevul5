int Ecma262Canonicalize::Convert(uchar c,
                      uchar n,
                      uchar* result,
                      bool* allow_caching_ptr) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupMapping<true>(kEcma262CanonicalizeTable0,
                                           kEcma262CanonicalizeTable0Size,
                                           kEcma262CanonicalizeMultiStrings0,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 1: return LookupMapping<true>(kEcma262CanonicalizeTable1,
                                           kEcma262CanonicalizeTable1Size,
                                           kEcma262CanonicalizeMultiStrings1,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 5: return LookupMapping<true>(kEcma262CanonicalizeTable5,
                                           kEcma262CanonicalizeTable5Size,
                                           kEcma262CanonicalizeMultiStrings5,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 7: return LookupMapping<true>(kEcma262CanonicalizeTable7,
                                           kEcma262CanonicalizeTable7Size,
                                           kEcma262CanonicalizeMultiStrings7,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    default: return 0;
  }
}