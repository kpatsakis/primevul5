void FoFiType1C::convertToCIDType0(char *psName, int *codeMap, int nCodes,
				   FoFiOutputFunc outputFunc,
				   void *outputStream) {
  int *cidMap;
  GooString *charStrings;
  int *charStringOffsets;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  int nCIDs, gdBytes;
  GooString *buf;
  char buf2[256];
  GBool ok;
  int gid, offset, n, i, j, k;

  if (codeMap) {
    nCIDs = nCodes;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCodes; ++i) {
      if (codeMap[i] >= 0 && codeMap[i] < nGlyphs) {
	cidMap[i] = codeMap[i];
      } else {
	cidMap[i] = -1;
      }
    }
  } else if (topDict.firstOp == 0x0c1e) {
    nCIDs = 0;
    for (i = 0; i < nGlyphs && i < charsetLength; ++i) {
      if (charset[i] >= nCIDs) {
	nCIDs = charset[i] + 1;
      }
    }
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = -1;
    }
    for (i = 0; i < nGlyphs && i < charsetLength; ++i) {
      cidMap[charset[i]] = i;
    }
  } else {
    nCIDs = nGlyphs;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = i;
    }
  }

  charStrings = new GooString();
  charStringOffsets = (int *)gmallocn(nCIDs + 1, sizeof(int));
  for (i = 0; i < nCIDs; ++i) {
    charStringOffsets[i] = charStrings->getLength();
    if ((gid = cidMap[i]) >= 0) {
      ok = gTrue;
      getIndexVal(&charStringsIdx, gid, &val, &ok);
      if (ok) {
	getIndex(privateDicts[fdSelect ? fdSelect[gid] : 0].subrsOffset,
		 &subrIdx, &ok);
	if (!ok) {
	  subrIdx.pos = -1;
	}
	cvtGlyph(val.pos, val.len, charStrings,
		 &subrIdx, &privateDicts[fdSelect ? fdSelect[gid] : 0], gTrue);
      }
    }
  }
  charStringOffsets[nCIDs] = charStrings->getLength();

  i = (nCIDs + 1) * 5 + charStrings->getLength();
  if (i < 0x100) {
    gdBytes = 1;
  } else if (i < 0x10000) {
    gdBytes = 2;
  } else if (i < 0x1000000) {
    gdBytes = 3;
  } else {
    gdBytes = 4;
  }

  (*outputFunc)(outputStream, "/CIDInit /ProcSet findresource begin\n", 37);
  (*outputFunc)(outputStream, "20 dict begin\n", 14);
  (*outputFunc)(outputStream, "/CIDFontName /", 14);
  (*outputFunc)(outputStream, psName, strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/CIDFontType 0 def\n", 19);
  (*outputFunc)(outputStream, "/CIDSystemInfo 3 dict dup begin\n", 32);
  if (topDict.registrySID > 0 && topDict.orderingSID > 0) {
    ok = gTrue;
    getString(topDict.registrySID, buf2, &ok);
    if (ok) {
      (*outputFunc)(outputStream, "  /Registry (", 13);
      (*outputFunc)(outputStream, buf2, strlen(buf2));
      (*outputFunc)(outputStream, ") def\n", 6);
    }
    ok = gTrue;
    getString(topDict.orderingSID, buf2, &ok);
    if (ok) {
      (*outputFunc)(outputStream, "  /Ordering (", 13);
      (*outputFunc)(outputStream, buf2, strlen(buf2));
      (*outputFunc)(outputStream, ") def\n", 6);
    }
  } else {
    (*outputFunc)(outputStream, "  /Registry (Adobe) def\n", 24);
    (*outputFunc)(outputStream, "  /Ordering (Identity) def\n", 27);
  }
  buf = GooString::format("  /Supplement {0:d} def\n", topDict.supplement);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "end def\n", 8);
  if (topDict.hasFontMatrix) {
    buf = GooString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			  topDict.fontMatrix[0], topDict.fontMatrix[1],
			  topDict.fontMatrix[2], topDict.fontMatrix[3],
			  topDict.fontMatrix[4], topDict.fontMatrix[5]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  } else if (privateDicts[0].hasFontMatrix) {
    (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  } else {
    (*outputFunc)(outputStream,
		  "/FontMatrix [0.001 0 0 0.001 0 0] def\n", 38);
  }
  buf = GooString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] def\n",
			topDict.fontBBox[0], topDict.fontBBox[1],
			topDict.fontBBox[2], topDict.fontBBox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FontInfo 1 dict dup begin\n", 27);
  (*outputFunc)(outputStream, "  /FSType 8 def\n", 16);
  (*outputFunc)(outputStream, "end def\n", 8);

  buf = GooString::format("/CIDCount {0:d} def\n", nCIDs);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FDBytes 1 def\n", 15);
  buf = GooString::format("/GDBytes {0:d} def\n", gdBytes);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/CIDMapOffset 0 def\n", 20);
  if (topDict.paintType != 0) {
    buf = GooString::format("/PaintType {0:d} def\n", topDict.paintType);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    buf = GooString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }

  buf = GooString::format("/FDArray {0:d} array\n", nFDs);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  for (i = 0; i < nFDs; ++i) {
    buf = GooString::format("dup {0:d} 10 dict begin\n", i);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
    if (privateDicts[i].hasFontMatrix) {
      buf = GooString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			    privateDicts[i].fontMatrix[0],
			    privateDicts[i].fontMatrix[1],
			    privateDicts[i].fontMatrix[2],
			    privateDicts[i].fontMatrix[3],
			    privateDicts[i].fontMatrix[4],
			    privateDicts[i].fontMatrix[5]);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    } else {
      (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
    }
    buf = GooString::format("/PaintType {0:d} def\n", topDict.paintType);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/Private 32 dict begin\n", 23);
    if (privateDicts[i].nBlueValues) {
      (*outputFunc)(outputStream, "/BlueValues [", 13);
      for (j = 0; j < privateDicts[i].nBlueValues; ++j) {
	buf = GooString::format("{0:s}{1:d}",
			      j > 0 ? " " : "", privateDicts[i].blueValues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nOtherBlues) {
      (*outputFunc)(outputStream, "/OtherBlues [", 13);
      for (j = 0; j < privateDicts[i].nOtherBlues; ++j) {
	buf = GooString::format("{0:s}{1:d}",
			      j > 0 ? " " : "", privateDicts[i].otherBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nFamilyBlues) {
      (*outputFunc)(outputStream, "/FamilyBlues [", 14);
      for (j = 0; j < privateDicts[i].nFamilyBlues; ++j) {
	buf = GooString::format("{0:s}{1:d}",
			      j > 0 ? " " : "",
			      privateDicts[i].familyBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nFamilyOtherBlues) {
      (*outputFunc)(outputStream, "/FamilyOtherBlues [", 19);
      for (j = 0; j < privateDicts[i].nFamilyOtherBlues; ++j) {
	buf = GooString::format("{0:s}{1:d}", j > 0 ? " " : "",
			      privateDicts[i].familyOtherBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].blueScale != 0.039625) {
      buf = GooString::format("/BlueScale {0:.4g} def\n",
			    privateDicts[i].blueScale);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].blueShift != 7) {
      buf = GooString::format("/BlueShift {0:d} def\n",
			    privateDicts[i].blueShift);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].blueFuzz != 1) {
      buf = GooString::format("/BlueFuzz {0:d} def\n", privateDicts[i].blueFuzz);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].hasStdHW) {
      buf = GooString::format("/StdHW [{0:.4g}] def\n", privateDicts[i].stdHW);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].hasStdVW) {
      buf = GooString::format("/StdVW [{0:.4g}] def\n", privateDicts[i].stdVW);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].nStemSnapH) {
      (*outputFunc)(outputStream, "/StemSnapH [", 12);
      for (j = 0; j < privateDicts[i].nStemSnapH; ++j) {
	buf = GooString::format("{0:s}{1:.4g}",
			      j > 0 ? " " : "", privateDicts[i].stemSnapH[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nStemSnapV) {
      (*outputFunc)(outputStream, "/StemSnapV [", 12);
      for (j = 0; j < privateDicts[i].nStemSnapV; ++j) {
	buf = GooString::format("{0:s}{1:.4g}",
			      j > 0 ? " " : "", privateDicts[i].stemSnapV[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].hasForceBold) {
      buf = GooString::format("/ForceBold {0:s} def\n",
			    privateDicts[i].forceBold ? "true" : "false");
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].forceBoldThreshold != 0) {
      buf = GooString::format("/ForceBoldThreshold {0:.4g} def\n",
			    privateDicts[i].forceBoldThreshold);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].languageGroup != 0) {
      buf = GooString::format("/LanguageGroup {0:d} def\n",
			    privateDicts[i].languageGroup);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].expansionFactor != 0.06) {
      buf = GooString::format("/ExpansionFactor {0:.4g} def\n",
			    privateDicts[i].expansionFactor);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "currentdict end def\n", 20);
    (*outputFunc)(outputStream, "currentdict end put\n", 20);
  }
  (*outputFunc)(outputStream, "def\n", 4);

  offset = (nCIDs + 1) * (1 + gdBytes);
  buf = GooString::format("(Hex) {0:d} StartData\n",
			offset + charStrings->getLength());
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;

  for (i = 0; i <= nCIDs; i += 6) {
    for (j = 0; j < 6 && i+j <= nCIDs; ++j) {
      if (i+j < nCIDs && cidMap[i+j] >= 0 && fdSelect) {
	buf2[0] = (char)fdSelect[cidMap[i+j]];
      } else {
	buf2[0] = (char)0;
      }
      n = offset + charStringOffsets[i+j];
      for (k = gdBytes; k >= 1; --k) {
	buf2[k] = (char)(n & 0xff);
	n >>= 8;
      }
      for (k = 0; k <= gdBytes; ++k) {
	buf = GooString::format("{0:02x}", buf2[k] & 0xff);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
    }
    (*outputFunc)(outputStream, "\n", 1);
  }

  n = charStrings->getLength();
  for (i = 0; i < n; i += 32) {
    for (j = 0; j < 32 && i+j < n; ++j) {
      buf = GooString::format("{0:02x}", charStrings->getChar(i+j) & 0xff);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (i + 32 >= n) {
      (*outputFunc)(outputStream, ">", 1);
    }
    (*outputFunc)(outputStream, "\n", 1);
  }

  gfree(charStringOffsets);
  delete charStrings;
  gfree(cidMap);
}
