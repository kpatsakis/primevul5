int JBIG2MMRDecoder::getBlackCode() {
  const CCITTCode *p;
  Guint code;

  if (bufLen == 0) {
    buf = str->getChar() & 0xff;
    bufLen = 8;
    ++nBytesRead;
  }
  while (1) {
    if (bufLen >= 10 && ((buf >> (bufLen - 6)) & 0x3f) == 0) {
      if (bufLen <= 13) {
	code = buf << (13 - bufLen);
      } else {
	code = buf >> (bufLen - 13);
      }
      p = &blackTab1[code & 0x7f];
    } else if (bufLen >= 7 && ((buf >> (bufLen - 4)) & 0x0f) == 0 &&
	       ((buf >> (bufLen - 6)) & 0x03) != 0) {
      if (bufLen <= 12) {
	code = buf << (12 - bufLen);
      } else {
	code = buf >> (bufLen - 12);
      }
      p = &blackTab2[(code & 0xff) - 64];
    } else {
      if (bufLen <= 6) {
	code = buf << (6 - bufLen);
      } else {
	code = buf >> (bufLen - 6);
      }
      p = &blackTab3[code & 0x3f];
    }
    if (p->bits > 0 && p->bits <= (int)bufLen) {
      bufLen -= p->bits;
      return p->n;
    }
    if (bufLen >= 13) {
      break;
    }
    buf = (buf << 8) | (str->getChar() & 0xff);
    bufLen += 8;
    ++nBytesRead;
  }
  error(str->getPos(), "Bad black code in JBIG2 MMR stream");
  --bufLen;
  return 1;
}
