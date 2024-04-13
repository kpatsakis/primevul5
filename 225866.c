bool WhiteSpace::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kWhiteSpaceTable0,
                                       kWhiteSpaceTable0Size,
                                       c);
    case 1: return LookupPredicate(kWhiteSpaceTable1,
                                       kWhiteSpaceTable1Size,
                                       c);
    default: return false;
  }
}