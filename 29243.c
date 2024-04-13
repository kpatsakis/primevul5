JBIG2Bitmap *JBIG2Stream::readGenericBitmap(GBool mmr, int w, int h,
					    int templ, GBool tpgdOn,
					    GBool useSkip, JBIG2Bitmap *skip,
					    int *atx, int *aty,
					    int mmrDataLength) {
  JBIG2Bitmap *bitmap;
  GBool ltp;
  Guint ltpCX, cx, cx0, cx1, cx2;
  JBIG2BitmapPtr cxPtr0 = {0}, cxPtr1 = {0};
  JBIG2BitmapPtr atPtr0 = {0}, atPtr1 = {0}, atPtr2 = {0}, atPtr3 = {0};
  int *refLine, *codingLine;
  int code1, code2, code3;
  int x, y, a0, pix, i, refI, codingI;

  bitmap = new JBIG2Bitmap(0, w, h);
  bitmap->clearToZero();


  if (mmr) {

    mmrDecoder->reset();
    refLine = (int *)gmallocn(w + 2, sizeof(int));
    codingLine = (int *)gmallocn(w + 2, sizeof(int));
    codingLine[0] = codingLine[1] = w;

    for (y = 0; y < h; ++y) {

      for (i = 0; codingLine[i] < w; ++i) {
	refLine[i] = codingLine[i];
      }
      refLine[i] = refLine[i + 1] = w;

      refI = 0;     // b1 = refLine[refI]
      codingI = 0;  // a1 = codingLine[codingI]
      a0 = 0;
      do {
	code1 = mmrDecoder->get2DCode();
	switch (code1) {
	case twoDimPass:
	  if (refLine[refI] < w) {
	    a0 = refLine[refI + 1];
	    refI += 2;
	  }
	  break;
	case twoDimHoriz:
	  if (codingI & 1) {
	    code1 = 0;
	    do {
	      code1 += code3 = mmrDecoder->getBlackCode();
	    } while (code3 >= 64);
	    code2 = 0;
	    do {
	      code2 += code3 = mmrDecoder->getWhiteCode();
	    } while (code3 >= 64);
	  } else {
	    code1 = 0;
	    do {
	      code1 += code3 = mmrDecoder->getWhiteCode();
	    } while (code3 >= 64);
	    code2 = 0;
	    do {
	      code2 += code3 = mmrDecoder->getBlackCode();
	    } while (code3 >= 64);
	  }
	  if (code1 > 0 || code2 > 0) {
	    a0 = codingLine[codingI++] = a0 + code1;
	    a0 = codingLine[codingI++] = a0 + code2;
	    while (refLine[refI] <= a0 && refLine[refI] < w) {
	      refI += 2;
	    }
	  }
	  break;
	case twoDimVert0:
	  a0 = codingLine[codingI++] = refLine[refI];
	  if (refLine[refI] < w) {
	    ++refI;
	  }
	  break;
	case twoDimVertR1:
	  a0 = codingLine[codingI++] = refLine[refI] + 1;
	  if (refLine[refI] < w) {
	    ++refI;
	    while (refLine[refI] <= a0 && refLine[refI] < w) {
	      refI += 2;
	    }
	  }
	  break;
	case twoDimVertR2:
	  a0 = codingLine[codingI++] = refLine[refI] + 2;
	  if (refLine[refI] < w) {
	    ++refI;
	    while (refLine[refI] <= a0 && refLine[refI] < w) {
	      refI += 2;
	    }
	  }
	  break;
	case twoDimVertR3:
	  a0 = codingLine[codingI++] = refLine[refI] + 3;
	  if (refLine[refI] < w) {
	    ++refI;
	    while (refLine[refI] <= a0 && refLine[refI] < w) {
	      refI += 2;
	    }
	  }
	  break;
	case twoDimVertL1:
	  a0 = codingLine[codingI++] = refLine[refI] - 1;
	  if (refI > 0) {
	    --refI;
	  } else {
	    ++refI;
	  }
	  while (refLine[refI] <= a0 && refLine[refI] < w) {
	    refI += 2;
	  }
	  break;
	case twoDimVertL2:
	  a0 = codingLine[codingI++] = refLine[refI] - 2;
	  if (refI > 0) {
	    --refI;
	  } else {
	    ++refI;
	  }
	  while (refLine[refI] <= a0 && refLine[refI] < w) {
	    refI += 2;
	  }
	  break;
	case twoDimVertL3:
	  a0 = codingLine[codingI++] = refLine[refI] - 3;
	  if (refI > 0) {
	    --refI;
	  } else {
	    ++refI;
	  }
	  while (refLine[refI] <= a0 && refLine[refI] < w) {
	    refI += 2;
	  }
	  break;
	default:
	  error(getPos(), "Illegal code in JBIG2 MMR bitmap data");
	  break;
	}
      } while (a0 < w);
      codingLine[codingI++] = w;

      i = 0;
      while (codingLine[i] < w) {
	for (x = codingLine[i]; x < codingLine[i+1]; ++x) {
	  bitmap->setPixel(x, y);
	}
	i += 2;
      }
    }

    if (mmrDataLength >= 0) {
      mmrDecoder->skipTo(mmrDataLength);
    } else {
      if (mmrDecoder->get24Bits() != 0x001001) {
	error(getPos(), "Missing EOFB in JBIG2 MMR bitmap data");
      }
    }

    gfree(refLine);
    gfree(codingLine);


  } else {
    ltpCX = 0; // make gcc happy
    if (tpgdOn) {
      switch (templ) {
      case 0:
	ltpCX = 0x3953; // 001 11001 0101 0011
	break;
      case 1:
	ltpCX = 0x079a; // 0011 11001 101 0
	break;
      case 2:
	ltpCX = 0x0e3; // 001 1100 01 1
	break;
      case 3:
	ltpCX = 0x18a; // 01100 0101 1
	break;
      }
    }

    ltp = 0;
    cx = cx0 = cx1 = cx2 = 0; // make gcc happy
    for (y = 0; y < h; ++y) {

      if (tpgdOn) {
	if (arithDecoder->decodeBit(ltpCX, genericRegionStats)) {
	  ltp = !ltp;
	}
	if (ltp) {
	  bitmap->duplicateRow(y, y-1);
	  continue;
	}
      }

      switch (templ) {
      case 0:

	bitmap->getPixelPtr(0, y-2, &cxPtr0);
	cx0 = bitmap->nextPixel(&cxPtr0);
	cx0 = (cx0 << 1) | bitmap->nextPixel(&cxPtr0);
	bitmap->getPixelPtr(0, y-1, &cxPtr1);
	cx1 = bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx2 = 0;
	bitmap->getPixelPtr(atx[0], y + aty[0], &atPtr0);
	bitmap->getPixelPtr(atx[1], y + aty[1], &atPtr1);
	bitmap->getPixelPtr(atx[2], y + aty[2], &atPtr2);
	bitmap->getPixelPtr(atx[3], y + aty[3], &atPtr3);

	for (x = 0; x < w; ++x) {

	  cx = (cx0 << 13) | (cx1 << 8) | (cx2 << 4) |
	       (bitmap->nextPixel(&atPtr0) << 3) |
	       (bitmap->nextPixel(&atPtr1) << 2) |
	       (bitmap->nextPixel(&atPtr2) << 1) |
	       bitmap->nextPixel(&atPtr3);

	  if (useSkip && skip->getPixel(x, y)) {
	    pix = 0;

	  } else if ((pix = arithDecoder->decodeBit(cx, genericRegionStats))) {
	    bitmap->setPixel(x, y);
	  }

	  cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 0x07;
	  cx1 = ((cx1 << 1) | bitmap->nextPixel(&cxPtr1)) & 0x1f;
	  cx2 = ((cx2 << 1) | pix) & 0x0f;
	}
	break;

      case 1:

	bitmap->getPixelPtr(0, y-2, &cxPtr0);
	cx0 = bitmap->nextPixel(&cxPtr0);
	cx0 = (cx0 << 1) | bitmap->nextPixel(&cxPtr0);
	cx0 = (cx0 << 1) | bitmap->nextPixel(&cxPtr0);
	bitmap->getPixelPtr(0, y-1, &cxPtr1);
	cx1 = bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx2 = 0;
	bitmap->getPixelPtr(atx[0], y + aty[0], &atPtr0);

	for (x = 0; x < w; ++x) {

	  cx = (cx0 << 9) | (cx1 << 4) | (cx2 << 1) |
	       bitmap->nextPixel(&atPtr0);

	  if (useSkip && skip->getPixel(x, y)) {
	    pix = 0;

	  } else if ((pix = arithDecoder->decodeBit(cx, genericRegionStats))) {
	    bitmap->setPixel(x, y);
	  }

	  cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 0x0f;
	  cx1 = ((cx1 << 1) | bitmap->nextPixel(&cxPtr1)) & 0x1f;
	  cx2 = ((cx2 << 1) | pix) & 0x07;
	}
	break;

      case 2:

	bitmap->getPixelPtr(0, y-2, &cxPtr0);
	cx0 = bitmap->nextPixel(&cxPtr0);
	cx0 = (cx0 << 1) | bitmap->nextPixel(&cxPtr0);
	bitmap->getPixelPtr(0, y-1, &cxPtr1);
	cx1 = bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx2 = 0;
	bitmap->getPixelPtr(atx[0], y + aty[0], &atPtr0);

	for (x = 0; x < w; ++x) {

	  cx = (cx0 << 7) | (cx1 << 3) | (cx2 << 1) |
	       bitmap->nextPixel(&atPtr0);

	  if (useSkip && skip->getPixel(x, y)) {
	    pix = 0;

	  } else if ((pix = arithDecoder->decodeBit(cx, genericRegionStats))) {
	    bitmap->setPixel(x, y);
	  }

	  cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 0x07;
	  cx1 = ((cx1 << 1) | bitmap->nextPixel(&cxPtr1)) & 0x0f;
	  cx2 = ((cx2 << 1) | pix) & 0x03;
	}
	break;

      case 3:

	bitmap->getPixelPtr(0, y-1, &cxPtr1);
	cx1 = bitmap->nextPixel(&cxPtr1);
	cx1 = (cx1 << 1) | bitmap->nextPixel(&cxPtr1);
	cx2 = 0;
	bitmap->getPixelPtr(atx[0], y + aty[0], &atPtr0);

	for (x = 0; x < w; ++x) {

	  cx = (cx1 << 5) | (cx2 << 1) |
	       bitmap->nextPixel(&atPtr0);

	  if (useSkip && skip->getPixel(x, y)) {
	    pix = 0;

	  } else if ((pix = arithDecoder->decodeBit(cx, genericRegionStats))) {
	    bitmap->setPixel(x, y);
	  }

	  cx1 = ((cx1 << 1) | bitmap->nextPixel(&cxPtr1)) & 0x1f;
	  cx2 = ((cx2 << 1) | pix) & 0x0f;
	}
	break;
      }
    }
  }

  return bitmap;
}
