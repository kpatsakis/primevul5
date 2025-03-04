GBool JBIG2HuffmanDecoder::decodeInt(int *x, JBIG2HuffmanTable *table) {
  Guint i, len, prefix;

  i = 0;
  len = 0;
  prefix = 0;
  while (table[i].rangeLen != jbig2HuffmanEOT) {
    while (len < table[i].prefixLen) {
      prefix = (prefix << 1) | readBit();
      ++len;
    }
    if (prefix == table[i].prefix) {
      if (table[i].rangeLen == jbig2HuffmanOOB) {
	return gFalse;
      }
      if (table[i].rangeLen == jbig2HuffmanLOW) {
	*x = table[i].val - readBits(32);
      } else if (table[i].rangeLen > 0) {
	*x = table[i].val + readBits(table[i].rangeLen);
      } else {
	*x = table[i].val;
      }
      return gTrue;
    }
    ++i;
  }
  return gFalse;
}
