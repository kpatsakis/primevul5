bool Uppercase::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kUppercaseTable0,
                                       kUppercaseTable0Size,
                                       c);
    case 1: return LookupPredicate(kUppercaseTable1,
                                       kUppercaseTable1Size,
                                       c);
    case 5: return LookupPredicate(kUppercaseTable5,
                                       kUppercaseTable5Size,
                                       c);
    case 7: return LookupPredicate(kUppercaseTable7,
                                       kUppercaseTable7Size,
                                       c);
    default: return false;
  }
}