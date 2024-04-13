JBIG2Bitmap *JBIG2Stream::readGenericRefinementRegion(int w, int h,
						      int templ, GBool tpgrOn,
						      JBIG2Bitmap *refBitmap,
						      int refDX, int refDY,
						      int *atx, int *aty) {
  JBIG2Bitmap *bitmap;
  GBool ltp;
  Guint ltpCX, cx, cx0, cx2, cx3, cx4, tpgrCX0, tpgrCX1, tpgrCX2;
  JBIG2BitmapPtr cxPtr0 = {0};
  JBIG2BitmapPtr cxPtr1 = {0};
  JBIG2BitmapPtr cxPtr2 = {0};
  JBIG2BitmapPtr cxPtr3 = {0};
  JBIG2BitmapPtr cxPtr4 = {0};
  JBIG2BitmapPtr cxPtr5 = {0};
  JBIG2BitmapPtr cxPtr6 = {0};
  JBIG2BitmapPtr tpgrCXPtr0 = {0};
  JBIG2BitmapPtr tpgrCXPtr1 = {0};
  JBIG2BitmapPtr tpgrCXPtr2 = {0};
  int x, y, pix;

  bitmap = new JBIG2Bitmap(0, w, h);
  if (!bitmap->isOk())
  {
    delete bitmap;
    return NULL;
  }
  bitmap->clearToZero();

  if (templ) {
    ltpCX = 0x008;
  } else {
    ltpCX = 0x0010;
  }

  ltp = 0;
  for (y = 0; y < h; ++y) {

    if (templ) {

      bitmap->getPixelPtr(0, y-1, &cxPtr0);
      cx0 = bitmap->nextPixel(&cxPtr0);
      bitmap->getPixelPtr(-1, y, &cxPtr1);
      refBitmap->getPixelPtr(-refDX, y-1-refDY, &cxPtr2);
      refBitmap->getPixelPtr(-1-refDX, y-refDY, &cxPtr3);
      cx3 = refBitmap->nextPixel(&cxPtr3);
      cx3 = (cx3 << 1) | refBitmap->nextPixel(&cxPtr3);
      refBitmap->getPixelPtr(-refDX, y+1-refDY, &cxPtr4);
      cx4 = refBitmap->nextPixel(&cxPtr4);

      tpgrCX0 = tpgrCX1 = tpgrCX2 = 0; // make gcc happy
      if (tpgrOn) {
	refBitmap->getPixelPtr(-1-refDX, y-1-refDY, &tpgrCXPtr0);
	tpgrCX0 = refBitmap->nextPixel(&tpgrCXPtr0);
	tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
	tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
	refBitmap->getPixelPtr(-1-refDX, y-refDY, &tpgrCXPtr1);
	tpgrCX1 = refBitmap->nextPixel(&tpgrCXPtr1);
	tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
	tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
	refBitmap->getPixelPtr(-1-refDX, y+1-refDY, &tpgrCXPtr2);
	tpgrCX2 = refBitmap->nextPixel(&tpgrCXPtr2);
	tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
	tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
      }

      for (x = 0; x < w; ++x) {

	cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 7;
	cx3 = ((cx3 << 1) | refBitmap->nextPixel(&cxPtr3)) & 7;
	cx4 = ((cx4 << 1) | refBitmap->nextPixel(&cxPtr4)) & 3;

	if (tpgrOn) {
	  tpgrCX0 = ((tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0)) & 7;
	  tpgrCX1 = ((tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1)) & 7;
	  tpgrCX2 = ((tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2)) & 7;

	  if (arithDecoder->decodeBit(ltpCX, refinementRegionStats)) {
	    ltp = !ltp;
	  }
	  if (tpgrCX0 == 0 && tpgrCX1 == 0 && tpgrCX2 == 0) {
	    bitmap->clearPixel(x, y);
	    continue;
	  } else if (tpgrCX0 == 7 && tpgrCX1 == 7 && tpgrCX2 == 7) {
	    bitmap->setPixel(x, y);
	    continue;
	  }
	}

	cx = (cx0 << 7) | (bitmap->nextPixel(&cxPtr1) << 6) |
	     (refBitmap->nextPixel(&cxPtr2) << 5) |
	     (cx3 << 2) | cx4;

	if ((pix = arithDecoder->decodeBit(cx, refinementRegionStats))) {
	  bitmap->setPixel(x, y);
	}
      }

    } else {

      bitmap->getPixelPtr(0, y-1, &cxPtr0);
      cx0 = bitmap->nextPixel(&cxPtr0);
      bitmap->getPixelPtr(-1, y, &cxPtr1);
      refBitmap->getPixelPtr(-refDX, y-1-refDY, &cxPtr2);
      cx2 = refBitmap->nextPixel(&cxPtr2);
      refBitmap->getPixelPtr(-1-refDX, y-refDY, &cxPtr3);
      cx3 = refBitmap->nextPixel(&cxPtr3);
      cx3 = (cx3 << 1) | refBitmap->nextPixel(&cxPtr3);
      refBitmap->getPixelPtr(-1-refDX, y+1-refDY, &cxPtr4);
      cx4 = refBitmap->nextPixel(&cxPtr4);
      cx4 = (cx4 << 1) | refBitmap->nextPixel(&cxPtr4);
      bitmap->getPixelPtr(atx[0], y+aty[0], &cxPtr5);
      refBitmap->getPixelPtr(atx[1]-refDX, y+aty[1]-refDY, &cxPtr6);

      tpgrCX0 = tpgrCX1 = tpgrCX2 = 0; // make gcc happy
      if (tpgrOn) {
	refBitmap->getPixelPtr(-1-refDX, y-1-refDY, &tpgrCXPtr0);
	tpgrCX0 = refBitmap->nextPixel(&tpgrCXPtr0);
	tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
	tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
	refBitmap->getPixelPtr(-1-refDX, y-refDY, &tpgrCXPtr1);
	tpgrCX1 = refBitmap->nextPixel(&tpgrCXPtr1);
	tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
	tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
	refBitmap->getPixelPtr(-1-refDX, y+1-refDY, &tpgrCXPtr2);
	tpgrCX2 = refBitmap->nextPixel(&tpgrCXPtr2);
	tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
	tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
      }

      for (x = 0; x < w; ++x) {

	cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 3;
	cx2 = ((cx2 << 1) | refBitmap->nextPixel(&cxPtr2)) & 3;
	cx3 = ((cx3 << 1) | refBitmap->nextPixel(&cxPtr3)) & 7;
	cx4 = ((cx4 << 1) | refBitmap->nextPixel(&cxPtr4)) & 7;

	if (tpgrOn) {
	  tpgrCX0 = ((tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0)) & 7;
	  tpgrCX1 = ((tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1)) & 7;
	  tpgrCX2 = ((tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2)) & 7;

	  if (arithDecoder->decodeBit(ltpCX, refinementRegionStats)) {
	    ltp = !ltp;
	  }
	  if (tpgrCX0 == 0 && tpgrCX1 == 0 && tpgrCX2 == 0) {
	    bitmap->clearPixel(x, y);
	    continue;
	  } else if (tpgrCX0 == 7 && tpgrCX1 == 7 && tpgrCX2 == 7) {
	    bitmap->setPixel(x, y);
	    continue;
	  }
	}

	cx = (cx0 << 11) | (bitmap->nextPixel(&cxPtr1) << 10) |
	     (cx2 << 8) | (cx3 << 5) | (cx4 << 2) |
	     (bitmap->nextPixel(&cxPtr5) << 1) |
	     refBitmap->nextPixel(&cxPtr6);

	if ((pix = arithDecoder->decodeBit(cx, refinementRegionStats))) {
	  bitmap->setPixel(x, y);
	}
      }
    }
  }

  return bitmap;
}
