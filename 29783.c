void FoFiType1C::readFDSelect() {
  int fdSelectFmt, pos, nRanges, gid0, gid1, fd, i, j;

  fdSelect = (Guchar *)gmalloc(nGlyphs);
  if (topDict.fdSelectOffset == 0) {
    for (i = 0; i < nGlyphs; ++i) {
      fdSelect[i] = 0;
    }
  } else {
    pos = topDict.fdSelectOffset;
    fdSelectFmt = getU8(pos++, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if (fdSelectFmt == 0) {
      if (!checkRegion(pos, nGlyphs)) {
	parsedOk = gFalse;
	return;
      }
      memcpy(fdSelect, file + pos, nGlyphs);
    } else if (fdSelectFmt == 3) {
      nRanges = getU16BE(pos, &parsedOk);
      pos += 2;
      gid0 = getU16BE(pos, &parsedOk);
      pos += 2;
      for (i = 1; i <= nRanges; ++i) {
	fd = getU8(pos++, &parsedOk);
	gid1 = getU16BE(pos, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	pos += 2;
	if (gid0 > gid1 || gid1 > nGlyphs) {
	  parsedOk = gFalse;
	  return;
	}
	for (j = gid0; j < gid1; ++j) {
	  fdSelect[j] = fd;
	}
	gid0 = gid1;
      }
    } else {
      for (i = 0; i < nGlyphs; ++i) {
	fdSelect[i] = 0;
      }
    }
  }
}
