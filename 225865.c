bool Letter::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kLetterTable0,
                                       kLetterTable0Size,
                                       c);
    case 1: return LookupPredicate(kLetterTable1,
                                       kLetterTable1Size,
                                       c);
    case 2: return LookupPredicate(kLetterTable2,
                                       kLetterTable2Size,
                                       c);
    case 3: return LookupPredicate(kLetterTable3,
                                       kLetterTable3Size,
                                       c);
    case 4: return LookupPredicate(kLetterTable4,
                                       kLetterTable4Size,
                                       c);
    case 5: return LookupPredicate(kLetterTable5,
                                       kLetterTable5Size,
                                       c);
    case 6: return LookupPredicate(kLetterTable6,
                                       kLetterTable6Size,
                                       c);
    case 7: return LookupPredicate(kLetterTable7,
                                       kLetterTable7Size,
                                       c);
    default: return false;
  }
}