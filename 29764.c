void FoFiType1C::convertToType1(char *psName, const char **newEncoding, GBool ascii,
				FoFiOutputFunc outputFunc,
				void *outputStream) {
  int psNameLen;
  Type1CEexecBuf eb;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  GooString *buf;
  char buf2[256];
  const char **enc;
  GBool ok;
  int i;

  if (psName) {
    psNameLen = strlen(psName);
  } else {
    psName = name->getCString();
    psNameLen = name->getLength();
  }

  ok = gTrue;
  (*outputFunc)(outputStream, "%!FontType1-1.0: ", 17);
  (*outputFunc)(outputStream, psName, psNameLen);
  if (topDict.versionSID != 0) {
    getString(topDict.versionSID, buf2, &ok);
    (*outputFunc)(outputStream, buf2, strlen(buf2));
  }
  (*outputFunc)(outputStream, "\n", 1);
  (*outputFunc)(outputStream, "12 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontInfo 10 dict dup begin\n", 28);
  if (topDict.versionSID != 0) {
    (*outputFunc)(outputStream, "/version ", 9);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.noticeSID != 0) {
    getString(topDict.noticeSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Notice ", 8);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.copyrightSID != 0) {
    getString(topDict.copyrightSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Copyright ", 11);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.fullNameSID != 0) {
    getString(topDict.fullNameSID, buf2, &ok);
    (*outputFunc)(outputStream, "/FullName ", 10);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.familyNameSID != 0) {
    getString(topDict.familyNameSID, buf2, &ok);
    (*outputFunc)(outputStream, "/FamilyName ", 12);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.weightSID != 0) {
    getString(topDict.weightSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Weight ", 8);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.isFixedPitch) {
    (*outputFunc)(outputStream, "/isFixedPitch true def\n", 23);
  } else {
    (*outputFunc)(outputStream, "/isFixedPitch false def\n", 24);
  }
  buf = GooString::format("/ItalicAngle {0:.4g} def\n", topDict.italicAngle);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GooString::format("/UnderlinePosition {0:.4g} def\n",
			topDict.underlinePosition);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GooString::format("/UnderlineThickness {0:.4g} def\n",
			topDict.underlineThickness);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "end readonly def\n", 17);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, psNameLen);
  (*outputFunc)(outputStream, " def\n", 5);
  buf = GooString::format("/PaintType {0:d} def\n", topDict.paintType);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
  buf = GooString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] readonly def\n",
			topDict.fontMatrix[0], topDict.fontMatrix[1],
			topDict.fontMatrix[2], topDict.fontMatrix[3],
			topDict.fontMatrix[4], topDict.fontMatrix[5]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GooString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] readonly def\n",
			topDict.fontBBox[0], topDict.fontBBox[1],
			topDict.fontBBox[2], topDict.fontBBox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GooString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  if (topDict.uniqueID != 0) {
    buf = GooString::format("/UniqueID {0:d} def\n", topDict.uniqueID);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }

  (*outputFunc)(outputStream, "/Encoding ", 10);
  if (!newEncoding && encoding == fofiType1StandardEncoding) {
    (*outputFunc)(outputStream, "StandardEncoding def\n", 21);
  } else {
    (*outputFunc)(outputStream, "256 array\n", 10);
    (*outputFunc)(outputStream,
		  "0 1 255 {1 index exch /.notdef put} for\n", 40);
    enc = newEncoding ? newEncoding : (const char **)encoding;
    for (i = 0; i < 256; ++i) {
      if (enc[i]) {
	buf = GooString::format("dup {0:d} /{1:s} put\n", i, enc[i]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
    }
    (*outputFunc)(outputStream, "readonly def\n", 13);
  }
  (*outputFunc)(outputStream, "currentdict end\n", 16);

  (*outputFunc)(outputStream, "currentfile eexec\n", 18);
  eb.outputFunc = outputFunc;
  eb.outputStream = outputStream;
  eb.ascii = ascii;
  eb.r1 = 55665;
  eb.line = 0;

  eexecWrite(&eb, "\x83\xca\x73\xd5");
  eexecWrite(&eb, "dup /Private 32 dict dup begin\n");
  eexecWrite(&eb, "/RD {string currentfile exch readstring pop}"
	     " executeonly def\n");
  eexecWrite(&eb, "/ND {noaccess def} executeonly def\n");
  eexecWrite(&eb, "/NP {noaccess put} executeonly def\n");
  eexecWrite(&eb, "/MinFeature {16 16} def\n");
  eexecWrite(&eb, "/password 5839 def\n");
  if (privateDicts[0].nBlueValues) {
    eexecWrite(&eb, "/BlueValues [");
    for (i = 0; i < privateDicts[0].nBlueValues; ++i) {
      buf = GooString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].blueValues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nOtherBlues) {
    eexecWrite(&eb, "/OtherBlues [");
    for (i = 0; i < privateDicts[0].nOtherBlues; ++i) {
      buf = GooString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].otherBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nFamilyBlues) {
    eexecWrite(&eb, "/FamilyBlues [");
    for (i = 0; i < privateDicts[0].nFamilyBlues; ++i) {
      buf = GooString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].familyBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nFamilyOtherBlues) {
    eexecWrite(&eb, "/FamilyOtherBlues [");
    for (i = 0; i < privateDicts[0].nFamilyOtherBlues; ++i) {
      buf = GooString::format("{0:s}{1:d}", i > 0 ? " " : "",
			    privateDicts[0].familyOtherBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].blueScale != 0.039625) {
    buf = GooString::format("/BlueScale {0:.4g} def\n",
			  privateDicts[0].blueScale);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].blueShift != 7) {
    buf = GooString::format("/BlueShift {0:d} def\n", privateDicts[0].blueShift);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].blueFuzz != 1) {
    buf = GooString::format("/BlueFuzz {0:d} def\n", privateDicts[0].blueFuzz);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].hasStdHW) {
    buf = GooString::format("/StdHW [{0:.4g}] def\n", privateDicts[0].stdHW);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].hasStdVW) {
    buf = GooString::format("/StdVW [{0:.4g}] def\n", privateDicts[0].stdVW);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].nStemSnapH) {
    eexecWrite(&eb, "/StemSnapH [");
    for (i = 0; i < privateDicts[0].nStemSnapH; ++i) {
      buf = GooString::format("{0:s}{1:.4g}",
			    i > 0 ? " " : "", privateDicts[0].stemSnapH[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nStemSnapV) {
    eexecWrite(&eb, "/StemSnapV [");
    for (i = 0; i < privateDicts[0].nStemSnapV; ++i) {
      buf = GooString::format("{0:s}{1:.4g}",
			    i > 0 ? " " : "", privateDicts[0].stemSnapV[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].hasForceBold) {
    buf = GooString::format("/ForceBold {0:s} def\n",
			  privateDicts[0].forceBold ? "true" : "false");
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].forceBoldThreshold != 0) {
    buf = GooString::format("/ForceBoldThreshold {0:.4g} def\n",
			  privateDicts[0].forceBoldThreshold);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].languageGroup != 0) {
    buf = GooString::format("/LanguageGroup {0:d} def\n",
			  privateDicts[0].languageGroup);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].expansionFactor != 0.06) {
    buf = GooString::format("/ExpansionFactor {0:.4g} def\n",
			  privateDicts[0].expansionFactor);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }

  ok = gTrue;
  getIndex(privateDicts[0].subrsOffset, &subrIdx, &ok);
  if (!ok) {
    subrIdx.pos = -1;
  }

  buf = GooString::format("2 index /CharStrings {0:d} dict dup begin\n",
			nGlyphs);
  eexecWrite(&eb, buf->getCString());
  delete buf;
  for (i = 0; i < nGlyphs; ++i) {
    ok = gTrue;
    getIndexVal(&charStringsIdx, i, &val, &ok);
    if (ok && i < charsetLength) {
      getString(charset[i], buf2, &ok);
      if (ok) {
	eexecCvtGlyph(&eb, buf2, val.pos, val.len, &subrIdx, &privateDicts[0]);
      }
    }
  }
  eexecWrite(&eb, "end\n");
  eexecWrite(&eb, "end\n");
  eexecWrite(&eb, "readonly put\n");
  eexecWrite(&eb, "noaccess put\n");
  eexecWrite(&eb, "dup /FontName get exch definefont pop\n");
  eexecWrite(&eb, "mark currentfile closefile\n");

  if (ascii && eb.line > 0) {
    (*outputFunc)(outputStream, "\n", 1);
  }
  for (i = 0; i < 8; ++i) {
    (*outputFunc)(outputStream, "0000000000000000000000000000000000000000000000000000000000000000\n", 65);
  }
  (*outputFunc)(outputStream, "cleartomark\n", 12);
}
