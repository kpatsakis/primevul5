void GfxColorSpace::getGrayLine(Guchar *in, unsigned char *out, int length) {
  int i, j, n;
  GfxColor color;
  GfxGray gray;

  n = getNComps();
  for (i = 0; i < length; i++) {
    
    for (j = 0; j < n; j++)
      color.c[j] = in[i * n + j] * 256;

    getGray (&color, &gray);
    out[i] = colToByte(gray);
  }
}
