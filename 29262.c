static void blendXor(SplashColorPtr src, SplashColorPtr dest,
		     SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = src[i] ^ dest[i];
  }
}
