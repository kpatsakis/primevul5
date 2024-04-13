bool CombiningMark::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kCombiningMarkTable0,
                                       kCombiningMarkTable0Size,
                                       c);
    case 1: return LookupPredicate(kCombiningMarkTable1,
                                       kCombiningMarkTable1Size,
                                       c);
    case 5: return LookupPredicate(kCombiningMarkTable5,
                                       kCombiningMarkTable5Size,
                                       c);
    case 7: return LookupPredicate(kCombiningMarkTable7,
                                       kCombiningMarkTable7Size,
                                       c);
    default: return false;
  }
}