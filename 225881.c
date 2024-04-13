int ToUppercase::Convert(uchar c,
                      uchar n,
                      uchar* result,
                      bool* allow_caching_ptr) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupMapping<true>(kToUppercaseTable0,
                                           kToUppercaseTable0Size,
                                           kToUppercaseMultiStrings0,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 1: return LookupMapping<true>(kToUppercaseTable1,
                                           kToUppercaseTable1Size,
                                           kToUppercaseMultiStrings1,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 5: return LookupMapping<true>(kToUppercaseTable5,
                                           kToUppercaseTable5Size,
                                           kToUppercaseMultiStrings5,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    case 7: return LookupMapping<true>(kToUppercaseTable7,
                                           kToUppercaseTable7Size,
                                           kToUppercaseMultiStrings7,
                                           c,
                                           n,
                                           result,
                                           allow_caching_ptr);
    default: return 0;
  }
}