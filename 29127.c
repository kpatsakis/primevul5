void GfxColorSpace::getRGBLine(Guchar *in, unsigned int *out, int length) {
  int i, j, n;
  GfxColor color;
  GfxRGB rgb;

  n = getNComps();
  for (i = 0; i < length; i++) {
    
    for (j = 0; j < n; j++)
      color.c[j] = in[i * n + j] * 256;

    getRGB (&color, &rgb);
    out[i] =
	((int) colToByte(rgb.r) << 16) |
	((int) colToByte(rgb.g) << 8) |
	((int) colToByte(rgb.b) << 0);
  }
}
