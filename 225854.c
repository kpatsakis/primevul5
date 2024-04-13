int ToLowercase::Convert(uchar c,
                      uchar n,
                      uchar* result,
                      bool* allow_caching_ptr) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupMapping<true>(kToLowercaseTable0,
                                           kToLowercaseTable0Size,
                                           kToLowercaseMultiStrings0,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 1: return LookupMapping<true>(kToLowercaseTable1,
                                           kToLowercaseTable1Size,
                                           kToLowercaseMultiStrings1,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 5: return LookupMapping<true>(kToLowercaseTable5,
                                           kToLowercaseTable5Size,
                                           kToLowercaseMultiStrings5,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 7: return LookupMapping<true>(kToLowercaseTable7,
                                           kToLowercaseTable7Size,
                                           kToLowercaseMultiStrings7,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    default: return 0;
  }
}