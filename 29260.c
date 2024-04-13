Splash::Splash(SplashBitmap *bitmapA, GBool vectorAntialiasA,
	       SplashScreenParams *screenParams) {
  int i;

  bitmap = bitmapA;
  vectorAntialias = vectorAntialiasA;
  state = new SplashState(bitmap->width, bitmap->height, vectorAntialias,
			  screenParams);
  if (vectorAntialias) {
    aaBuf = new SplashBitmap(splashAASize * bitmap->width, splashAASize,
			     1, splashModeMono1, gFalse);
    for (i = 0; i <= splashAASize * splashAASize; ++i) {
      aaGamma[i] = splashPow((SplashCoord)i /
			       (SplashCoord)(splashAASize * splashAASize),
			     1.5);
    }
  } else {
    aaBuf = NULL;
  }
  clearModRegion();
  debugMode = gFalse;
}
