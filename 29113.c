void GfxDeviceRGBColorSpace::getGrayLine(Guchar *in, Guchar *out, int length) {
  int i;

  for (i = 0; i < length; i++) {
    out[i] = 
      (in[i * 3 + 0] * 19595 + 
       in[i * 3 + 1] * 38469 + 
       in[i * 3 + 2] * 7472) / 65536;
  }
}
