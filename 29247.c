void JBIG2Stream::readHalftoneRegionSeg(Guint segNum, GBool imm,
					GBool lossless, Guint length,
					Guint *refSegs, Guint nRefSegs) {
  JBIG2Bitmap *bitmap;
  JBIG2Segment *seg;
  JBIG2PatternDict *patternDict;
  JBIG2Bitmap *skipBitmap;
  Guint *grayImg;
  JBIG2Bitmap *grayBitmap;
  JBIG2Bitmap *patternBitmap;
  Guint w, h, x, y, segInfoFlags, extCombOp;
  Guint flags, mmr, templ, enableSkip, combOp;
  Guint gridW, gridH, stepX, stepY, patW, patH;
  int atx[4], aty[4];
  int gridX, gridY, xx, yy, bit, j;
  Guint bpp, m, n, i;

  if (!readULong(&w) || !readULong(&h) ||
      !readULong(&x) || !readULong(&y) ||
      !readUByte(&segInfoFlags)) {
    goto eofError;
  }
  extCombOp = segInfoFlags & 7;

  if (!readUByte(&flags)) {
    goto eofError;
  }
  mmr = flags & 1;
  templ = (flags >> 1) & 3;
  enableSkip = (flags >> 3) & 1;
  combOp = (flags >> 4) & 7;
  if (!readULong(&gridW) || !readULong(&gridH) ||
      !readLong(&gridX) || !readLong(&gridY) ||
      !readUWord(&stepX) || !readUWord(&stepY)) {
    goto eofError;
  }
  if (w == 0 || h == 0 || w >= INT_MAX / h) {
    error(getPos(), "Bad bitmap size in JBIG2 halftone segment");
    return;
  }
  if (gridH == 0 || gridW >= INT_MAX / gridH) {
    error(getPos(), "Bad grid size in JBIG2 halftone segment");
    return;
  }

  if (nRefSegs != 1) {
    error(getPos(), "Bad symbol dictionary reference in JBIG2 halftone segment");
    return;
  }
  seg = findSegment(refSegs[0]);
  if (seg == NULL || seg->getType() != jbig2SegPatternDict) {
    error(getPos(), "Bad symbol dictionary reference in JBIG2 halftone segment");
    return;
  }

  patternDict = (JBIG2PatternDict *)seg;
  bpp = 0;
  i = 1;
  while (i < patternDict->getSize()) {
    ++bpp;
    i <<= 1;
  }
  patW = patternDict->getBitmap(0)->getWidth();
  patH = patternDict->getBitmap(0)->getHeight();

  if (!mmr) {
    resetGenericStats(templ, NULL);
    arithDecoder->start();
  }

  bitmap = new JBIG2Bitmap(segNum, w, h);
  if (flags & 0x80) { // HDEFPIXEL
    bitmap->clearToOne();
  } else {
    bitmap->clearToZero();
  }

  skipBitmap = NULL;
  if (enableSkip) {
    skipBitmap = new JBIG2Bitmap(0, gridW, gridH);
    skipBitmap->clearToZero();
    for (m = 0; m < gridH; ++m) {
      for (n = 0; n < gridW; ++n) {
	xx = gridX + m * stepY + n * stepX;
	yy = gridY + m * stepX - n * stepY;
	if (((xx + (int)patW) >> 8) <= 0 || (xx >> 8) >= (int)w ||
	    ((yy + (int)patH) >> 8) <= 0 || (yy >> 8) >= (int)h) {
	  skipBitmap->setPixel(n, m);
	}
      }
    }
  }

  grayImg = (Guint *)gmallocn(gridW * gridH, sizeof(Guint));
  memset(grayImg, 0, gridW * gridH * sizeof(Guint));
  atx[0] = templ <= 1 ? 3 : 2;  aty[0] = -1;
  atx[1] = -3;                  aty[1] = -1;
  atx[2] =  2;                  aty[2] = -2;
  atx[3] = -2;                  aty[3] = -2;
  for (j = bpp - 1; j >= 0; --j) {
    grayBitmap = readGenericBitmap(mmr, gridW, gridH, templ, gFalse,
				   enableSkip, skipBitmap, atx, aty, -1);
    i = 0;
    for (m = 0; m < gridH; ++m) {
      for (n = 0; n < gridW; ++n) {
	bit = grayBitmap->getPixel(n, m) ^ (grayImg[i] & 1);
	grayImg[i] = (grayImg[i] << 1) | bit;
	++i;
      }
    }
    delete grayBitmap;
  }

  i = 0;
  for (m = 0; m < gridH; ++m) {
    xx = gridX + m * stepY;
    yy = gridY + m * stepX;
    for (n = 0; n < gridW; ++n) {
      if (!(enableSkip && skipBitmap->getPixel(n, m))) {
	patternBitmap = patternDict->getBitmap(grayImg[i]);
	bitmap->combine(patternBitmap, xx >> 8, yy >> 8, combOp);
      }
      xx += stepX;
      yy -= stepY;
      ++i;
    }
  }

  gfree(grayImg);
  if (skipBitmap) {
    delete skipBitmap;
  }

  if (imm) {
    if (pageH == 0xffffffff && y + h > curPageH) {
      pageBitmap->expand(y + h, pageDefPixel);
    }
    pageBitmap->combine(bitmap, x, y, extCombOp);
    delete bitmap;

  } else {
    segments->append(bitmap);
  }

  return;

 eofError:
  error(getPos(), "Unexpected EOF in JBIG2 stream");
}
