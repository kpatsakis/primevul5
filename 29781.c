GBool FoFiType1C::readCharset() {
  int charsetFormat, c, pos;
  int nLeft, i, j;

  if (topDict.charsetOffset == 0) {
    charset = fofiType1CISOAdobeCharset;
    charsetLength = sizeof(fofiType1CISOAdobeCharset) / sizeof(Gushort);
  } else if (topDict.charsetOffset == 1) {
    charset = fofiType1CExpertCharset;
    charsetLength = sizeof(fofiType1CExpertCharset) / sizeof(Gushort);
  } else if (topDict.charsetOffset == 2) {
    charset = fofiType1CExpertSubsetCharset;
    charsetLength = sizeof(fofiType1CExpertSubsetCharset) / sizeof(Gushort);
  } else {
    charset = (Gushort *)gmallocn(nGlyphs, sizeof(Gushort));
    charsetLength = nGlyphs;
    for (i = 0; i < nGlyphs; ++i) {
      charset[i] = 0;
    }
    pos = topDict.charsetOffset;
    charsetFormat = getU8(pos++, &parsedOk);
    if (charsetFormat == 0) {
      for (i = 1; i < nGlyphs; ++i) {
	charset[i] = (Gushort)getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  break;
	}
      }
    } else if (charsetFormat == 1) {
      i = 1;
      while (i < nGlyphs) {
	c = getU16BE(pos, &parsedOk);
	pos += 2;
	nLeft = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  break;
	}
	for (j = 0; j <= nLeft && i < nGlyphs; ++j) {
	  charset[i++] = (Gushort)c++;
	}
      }
    } else if (charsetFormat == 2) {
      i = 1;
      while (i < nGlyphs) {
	c = getU16BE(pos, &parsedOk);
	pos += 2;
	nLeft = getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  break;
	}
	for (j = 0; j <= nLeft && i < nGlyphs; ++j) {
	  charset[i++] = (Gushort)c++;
	}
      }
    }
    if (!parsedOk) {
      gfree(charset);
      charset = NULL;
      charsetLength = 0;
      return gFalse;
    }
  }
  return gTrue;
}
