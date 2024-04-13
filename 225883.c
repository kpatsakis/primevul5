bool Number::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kNumberTable0,
                                       kNumberTable0Size,
                                       c);
    case 5: return LookupPredicate(kNumberTable5,
                                       kNumberTable5Size,
                                       c);
    case 7: return LookupPredicate(kNumberTable7,
                                       kNumberTable7Size,
                                       c);
    default: return false;
  }
}