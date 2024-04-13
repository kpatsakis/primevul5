int CanonicalizationRange::Convert(uchar c,
                      uchar n,
                      uchar* result,
                      bool* allow_caching_ptr) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupMapping<false>(kCanonicalizationRangeTable0,
                                           kCanonicalizationRangeTable0Size,
                                           kCanonicalizationRangeMultiStrings0,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 1: return LookupMapping<false>(kCanonicalizationRangeTable1,
                                           kCanonicalizationRangeTable1Size,
                                           kCanonicalizationRangeMultiStrings1,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 7: return LookupMapping<false>(kCanonicalizationRangeTable7,
                                           kCanonicalizationRangeTable7Size,
                                           kCanonicalizationRangeMultiStrings7,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    default: return 0;
  }
}