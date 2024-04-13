bool LineTerminator::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kLineTerminatorTable0,
                                       kLineTerminatorTable0Size,
                                       c);
    case 1: return LookupPredicate(kLineTerminatorTable1,
                                       kLineTerminatorTable1Size,
                                       c);
    default: return false;
  }
}