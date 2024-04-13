bool ConnectorPunctuation::Is(uchar c) {
  int chunk_index = c >> 13;
  switch (chunk_index) {
    case 0: return LookupPredicate(kConnectorPunctuationTable0,
                                       kConnectorPunctuationTable0Size,
                                       c);
    case 1: return LookupPredicate(kConnectorPunctuationTable1,
                                       kConnectorPunctuationTable1Size,
                                       c);
    case 7: return LookupPredicate(kConnectorPunctuationTable7,
                                       kConnectorPunctuationTable7Size,
                                       c);
    default: return false;
  }
}