int Ecma262UnCanonicalize::Convert(uchar c,
                      uchar n,
                      uchar* result,
                      bool* allow_caching_ptr) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupMapping<true>(kEcma262UnCanonicalizeTable0,
                                           kEcma262UnCanonicalizeTable0Size,
                                           kEcma262UnCanonicalizeMultiStrings0,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 1: return LookupMapping<true>(kEcma262UnCanonicalizeTable1,
                                           kEcma262UnCanonicalizeTable1Size,
                                           kEcma262UnCanonicalizeMultiStrings1,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 5: return LookupMapping<true>(kEcma262UnCanonicalizeTable5,
                                           kEcma262UnCanonicalizeTable5Size,
                                           kEcma262UnCanonicalizeMultiStrings5,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 7: return LookupMapping<true>(kEcma262UnCanonicalizeTable7,
                                           kEcma262UnCanonicalizeTable7Size,
                                           kEcma262UnCanonicalizeMultiStrings7,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    default: return 0;
  }
}