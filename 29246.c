void JBIG2Stream::readGenericRegionSeg(Guint segNum, GBool imm,
				       GBool lossless, Guint length) {
  JBIG2Bitmap *bitmap;
  Guint w, h, x, y, segInfoFlags, extCombOp;
  Guint flags, mmr, templ, tpgdOn;
  int atx[4], aty[4];

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
  tpgdOn = (flags >> 3) & 1;

  if (!mmr) {
    if (templ == 0) {
      if (!readByte(&atx[0]) ||
	  !readByte(&aty[0]) ||
	  !readByte(&atx[1]) ||
	  !readByte(&aty[1]) ||
	  !readByte(&atx[2]) ||
	  !readByte(&aty[2]) ||
	  !readByte(&atx[3]) ||
	  !readByte(&aty[3])) {
	goto eofError;
      }
    } else {
      if (!readByte(&atx[0]) ||
	  !readByte(&aty[0])) {
	goto eofError;
      }
    }
  }

  if (!mmr) {
    resetGenericStats(templ, NULL);
    arithDecoder->start();
  }

  bitmap = readGenericBitmap(mmr, w, h, templ, tpgdOn, gFalse,
			     NULL, atx, aty, mmr ? 0 : length - 18);

  if (imm) {
    if (pageH == 0xffffffff && y + h > curPageH) {
      pageBitmap->expand(y + h, pageDefPixel);
    }
    pageBitmap->combine(bitmap, x, y, extCombOp);
    delete bitmap;

  } else {
    bitmap->setSegNum(segNum);
    segments->append(bitmap);
  }

  return;

 eofError:
  error(getPos(), "Unexpected EOF in JBIG2 stream");
}
