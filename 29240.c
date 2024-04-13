void JBIG2Stream::readCodeTableSeg(Guint segNum, Guint length) {
  JBIG2HuffmanTable *huffTab;
  Guint flags, oob, prefixBits, rangeBits;
  int lowVal, highVal, val;
  Guint huffTabSize, i;

  if (!readUByte(&flags) || !readLong(&lowVal) || !readLong(&highVal)) {
    goto eofError;
  }
  oob = flags & 1;
  prefixBits = ((flags >> 1) & 7) + 1;
  rangeBits = ((flags >> 4) & 7) + 1;

  huffDecoder->reset();
  huffTabSize = 8;
  huffTab = (JBIG2HuffmanTable *)
                gmallocn(huffTabSize, sizeof(JBIG2HuffmanTable));
  i = 0;
  val = lowVal;
  while (val < highVal) {
    if (i == huffTabSize) {
      huffTabSize *= 2;
      huffTab = (JBIG2HuffmanTable *)
	            greallocn(huffTab, huffTabSize, sizeof(JBIG2HuffmanTable));
    }
    huffTab[i].val = val;
    huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
    huffTab[i].rangeLen = huffDecoder->readBits(rangeBits);
    val += 1 << huffTab[i].rangeLen;
    ++i;
  }
  if (i + oob + 3 > huffTabSize) {
    huffTabSize = i + oob + 3;
    huffTab = (JBIG2HuffmanTable *)
                  greallocn(huffTab, huffTabSize, sizeof(JBIG2HuffmanTable));
  }
  huffTab[i].val = lowVal - 1;
  huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
  huffTab[i].rangeLen = jbig2HuffmanLOW;
  ++i;
  huffTab[i].val = highVal;
  huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
  huffTab[i].rangeLen = 32;
  ++i;
  if (oob) {
    huffTab[i].val = 0;
    huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
    huffTab[i].rangeLen = jbig2HuffmanOOB;
    ++i;
  }
  huffTab[i].val = 0;
  huffTab[i].prefixLen = 0;
  huffTab[i].rangeLen = jbig2HuffmanEOT;
  huffDecoder->buildTable(huffTab, i);

  segments->append(new JBIG2CodeTable(segNum, huffTab));

  return;

 eofError:
  error(getPos(), "Unexpected EOF in JBIG2 stream");
}
