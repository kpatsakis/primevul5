void GfxDeviceGrayColorSpace::getRGBLine(Guchar *in, unsigned int *out,
					 int length) {
  int i;

  for (i = 0; i < length; i++)
    out[i] = (in[i] << 16) | (in[i] << 8) | (in[i] << 0);
}
