void JBIG2Stream::readPageInfoSeg(Guint length) {
  Guint xRes, yRes, flags, striping;

  if (!readULong(&pageW) || !readULong(&pageH) ||
      !readULong(&xRes) || !readULong(&yRes) ||
      !readUByte(&flags) || !readUWord(&striping)) {
    goto eofError;
  }
  pageDefPixel = (flags >> 2) & 1;
  defCombOp = (flags >> 3) & 3;

  if (pageH == 0xffffffff) {
    curPageH = striping & 0x7fff;
  } else {
    curPageH = pageH;
  }
  pageBitmap = new JBIG2Bitmap(0, pageW, curPageH);

  if (pageDefPixel) {
    pageBitmap->clearToOne();
  } else {
    pageBitmap->clearToZero();
  }

  return;

 eofError:
  error(getPos(), "Unexpected EOF in JBIG2 stream");
}
