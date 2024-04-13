GBool FoFiType1C::parse() {
  Type1CIndex fdIdx;
  Type1CIndexVal val;
  int i;

  parsedOk = gTrue;

  if (len > 0 && file[0] != '\x01') {
    ++file;
    --len;
  }

  getIndex(getU8(2, &parsedOk), &nameIdx, &parsedOk);
  getIndex(nameIdx.endPos, &topDictIdx, &parsedOk);
  getIndex(topDictIdx.endPos, &stringIdx, &parsedOk);
  getIndex(stringIdx.endPos, &gsubrIdx, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  gsubrBias = (gsubrIdx.len < 1240) ? 107
                                    : (gsubrIdx.len < 33900) ? 1131 : 32768;

  getIndexVal(&nameIdx, 0, &val, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  name = new GooString((char *)&file[val.pos], val.len);

  readTopDict();

  if (topDict.firstOp == 0x0c1e) {
    if (topDict.fdArrayOffset == 0) {
      nFDs = 1;
      privateDicts = (Type1CPrivateDict *)gmalloc(sizeof(Type1CPrivateDict));
      readPrivateDict(0, 0, &privateDicts[0]);
    } else {
      getIndex(topDict.fdArrayOffset, &fdIdx, &parsedOk);
      if (!parsedOk) {
	return gFalse;
      }
      nFDs = fdIdx.len;
      privateDicts = (Type1CPrivateDict *)
	                 gmallocn(nFDs, sizeof(Type1CPrivateDict));
      for (i = 0; i < nFDs; ++i) {
	getIndexVal(&fdIdx, i, &val, &parsedOk);
	if (!parsedOk) {
	  return gFalse;
	}
	readFD(val.pos, val.len, &privateDicts[i]);
      }
    }

  } else {
    nFDs = 1;
    privateDicts = (Type1CPrivateDict *)gmalloc(sizeof(Type1CPrivateDict));
    readPrivateDict(topDict.privateOffset, topDict.privateSize,
		    &privateDicts[0]);
  }

  if (!parsedOk) {
    return gFalse;
  }

  if (topDict.charStringsOffset <= 0) {
    parsedOk = gFalse;
    return gFalse;
  }
  getIndex(topDict.charStringsOffset, &charStringsIdx, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  nGlyphs = charStringsIdx.len;

  if (topDict.firstOp == 0x0c1e) {
    readFDSelect();
    if (!parsedOk) {
      return gFalse;
    }
  }

  if (!readCharset()) {
    parsedOk = gFalse;
    return gFalse;
  }

  if (topDict.firstOp != 0x0c14 && topDict.firstOp != 0x0c1e) {
    buildEncoding();
    if (!parsedOk) {
      return gFalse;
    }
  }

  return parsedOk;
}
