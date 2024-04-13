void JBIG2HuffmanDecoder::buildTable(JBIG2HuffmanTable *table, Guint len) {
  Guint i, j, k, prefix;
  JBIG2HuffmanTable tab;

  for (i = 0; i < len; ++i) {
    for (j = i; j < len && table[j].prefixLen == 0; ++j) ;
    if (j == len) {
      break;
    }
    for (k = j + 1; k < len; ++k) {
      if (table[k].prefixLen > 0 &&
	  table[k].prefixLen < table[j].prefixLen) {
	j = k;
      }
    }
    if (j != i) {
      tab = table[j];
      for (k = j; k > i; --k) {
	table[k] = table[k - 1];
      }
      table[i] = tab;
    }
  }
  table[i] = table[len];

  i = 0;
  prefix = 0;
  table[i++].prefix = prefix++;
  for (; table[i].rangeLen != jbig2HuffmanEOT; ++i) {
    prefix <<= table[i].prefixLen - table[i-1].prefixLen;
    table[i].prefix = prefix++;
  }
}
