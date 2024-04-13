bool Lowercase::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kLowercaseTable0,
                                       kLowercaseTable0Size,
                                       c);
    case 1: return LookupPredicate(kLowercaseTable1,
                                       kLowercaseTable1Size,
                                       c);
    case 5: return LookupPredicate(kLowercaseTable5,
                                       kLowercaseTable5Size,
                                       c);
    case 7: return LookupPredicate(kLowercaseTable7,
                                       kLowercaseTable7Size,
                                       c);
    default: return false;
  }
}