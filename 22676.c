onigenc_minimum_property_name_to_ctype(OnigEncoding enc, UChar* p, UChar* end)
{
  static const PosixBracketEntryType PBS[] = {
    PosixBracketEntryInit("Alnum",  ONIGENC_CTYPE_ALNUM),
    PosixBracketEntryInit("Alpha",  ONIGENC_CTYPE_ALPHA),
    PosixBracketEntryInit("Blank",  ONIGENC_CTYPE_BLANK),
    PosixBracketEntryInit("Cntrl",  ONIGENC_CTYPE_CNTRL),
    PosixBracketEntryInit("Digit",  ONIGENC_CTYPE_DIGIT),
    PosixBracketEntryInit("Graph",  ONIGENC_CTYPE_GRAPH),
    PosixBracketEntryInit("Lower",  ONIGENC_CTYPE_LOWER),
    PosixBracketEntryInit("Print",  ONIGENC_CTYPE_PRINT),
    PosixBracketEntryInit("Punct",  ONIGENC_CTYPE_PUNCT),
    PosixBracketEntryInit("Space",  ONIGENC_CTYPE_SPACE),
    PosixBracketEntryInit("Upper",  ONIGENC_CTYPE_UPPER),
    PosixBracketEntryInit("XDigit", ONIGENC_CTYPE_XDIGIT),
    PosixBracketEntryInit("ASCII",  ONIGENC_CTYPE_ASCII),
    PosixBracketEntryInit("Word",   ONIGENC_CTYPE_WORD),
  };

  const PosixBracketEntryType *pb, *pbe;
  int len;

  len = onigenc_strlen(enc, p, end);
  for (pbe = (pb = PBS) + sizeof(PBS)/sizeof(PBS[0]); pb < pbe; ++pb) {
    if (len == pb->len &&
        STRNCASECMP((char *)p, (char *)pb->name, len) == 0)
      return pb->ctype;
  }

  return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
}