void JBIG2Stream::readGenericRefinementRegionSeg(Guint segNum, GBool imm,
						 GBool lossless, Guint length,
						 Guint *refSegs,
						 Guint nRefSegs) {
  JBIG2Bitmap *bitmap, *refBitmap;
  Guint w, h, x, y, segInfoFlags, extCombOp;
  Guint flags, templ, tpgrOn;
  int atx[2], aty[2];
  JBIG2Segment *seg;

  if (!readULong(&w) || !readULong(&h) ||
      !readULong(&x) || !readULong(&y) ||
      !readUByte(&segInfoFlags)) {
    goto eofError;
  }
  extCombOp = segInfoFlags & 7;

  if (!readUByte(&flags)) {
    goto eofError;
  }
  templ = flags & 1;
  tpgrOn = (flags >> 1) & 1;

  if (!templ) {
    if (!readByte(&atx[0]) || !readByte(&aty[0]) ||
	!readByte(&atx[1]) || !readByte(&aty[1])) {
      goto eofError;
    }
  }

  if (nRefSegs == 0 || imm) {
    if (pageH == 0xffffffff && y + h > curPageH) {
      pageBitmap->expand(y + h, pageDefPixel);
    }
  }

  if (nRefSegs > 1) {
    error(getPos(), "Bad reference in JBIG2 generic refinement segment");
    return;
  }
  if (nRefSegs == 1) {
    seg = findSegment(refSegs[0]);
    if (seg == NULL || seg->getType() != jbig2SegBitmap) {
      error(getPos(), "Bad bitmap reference in JBIG2 generic refinement segment");
      return;
    }
    refBitmap = (JBIG2Bitmap *)seg;
  } else {
    refBitmap = pageBitmap->getSlice(x, y, w, h);
  }

  resetRefinementStats(templ, NULL);
  arithDecoder->start();

  bitmap = readGenericRefinementRegion(w, h, templ, tpgrOn,
				       refBitmap, 0, 0, atx, aty);

  if (imm) {
    pageBitmap->combine(bitmap, x, y, extCombOp);
    delete bitmap;

  } else {
    bitmap->setSegNum(segNum);
    segments->append(bitmap);
  }

  if (nRefSegs == 1) {
    discardSegment(refSegs[0]);
  } else {
    delete refBitmap;
  }

  return;

 eofError:
  error(getPos(), "Unexpected EOF in JBIG2 stream");
}
