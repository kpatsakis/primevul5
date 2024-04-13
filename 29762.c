void FoFiType1C::buildEncoding() {
  char buf[256];
  int nCodes, nRanges, encFormat;
  int pos, c, sid, nLeft, nSups, i, j;

  if (topDict.encodingOffset == 0) {
    encoding = (char **)fofiType1StandardEncoding;

  } else if (topDict.encodingOffset == 1) {
    encoding = (char **)fofiType1ExpertEncoding;

  } else {
    encoding = (char **)gmallocn(256, sizeof(char *));
    for (i = 0; i < 256; ++i) {
      encoding[i] = NULL;
    }
    pos = topDict.encodingOffset;
    encFormat = getU8(pos++, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if ((encFormat & 0x7f) == 0) {
      nCodes = 1 + getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      if (nCodes > nGlyphs) {
	nCodes = nGlyphs;
      }
      for (i = 1; i < nCodes && i < charsetLength; ++i) {
	c = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	if (encoding[c]) {
	  gfree(encoding[c]);
	}
	encoding[c] = copyString(getString(charset[i], buf, &parsedOk));
      }
    } else if ((encFormat & 0x7f) == 1) {
      nRanges = getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      nCodes = 1;
      for (i = 0; i < nRanges; ++i) {
	c = getU8(pos++, &parsedOk);
	nLeft = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	for (j = 0; j <= nLeft && nCodes < nGlyphs && nCodes < charsetLength; ++j) {
	  if (c < 256) {
	    if (encoding[c]) {
	      gfree(encoding[c]);
	    }
	    encoding[c] = copyString(getString(charset[nCodes], buf,
					       &parsedOk));
	  }
	  ++nCodes;
	  ++c;
	}
      }
    }
    if (encFormat & 0x80) {
      nSups = getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      for (i = 0; i < nSups; ++i) {
	c = getU8(pos++, &parsedOk);;
	if (!parsedOk) {
	  return;;
	}
	sid = getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  return;
	}
	if (encoding[c]) {
	  gfree(encoding[c]);
	}
	encoding[c] = copyString(getString(sid, buf, &parsedOk));
      }
    }
  }
}
