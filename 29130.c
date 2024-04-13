void GfxICCBasedColorSpace::getRGBLine(Guchar *in, unsigned int *out,
				       int length) {
#ifdef USE_CMS
  if (lineTransform != 0) {
    for (int i = 0;i < length;i++) {
	Guchar tmp[gfxColorMaxComps];

	lineTransform->doTransform(in,tmp,1);
	in += nComps;
	out[i] = (tmp[0] << 16) | (tmp[1] << 8) | tmp[2];
    }
  } else {
    alt->getRGBLine(in, out, length);
  }
#else
  alt->getRGBLine(in, out, length);
#endif
}
