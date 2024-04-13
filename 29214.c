void JBIG2Bitmap::expand(int newH, Guint pixel) {
  if (newH <= h || line <= 0 || newH >= (INT_MAX - 1) / line) {
    error(-1, "invalid width/height");
    gfree(data);
    data = NULL;
    return;
  }
  data = (Guchar *)grealloc(data, newH * line + 1);
  if (pixel) {
    memset(data + h * line, 0xff, (newH - h) * line);
  } else {
    memset(data + h * line, 0x00, (newH - h) * line);
  }
  h = newH;
  data[h * line] = 0;
}
