SplashError Splash::composite(SplashBitmap *src, int xSrc, int ySrc,
			      int xDest, int yDest, int w, int h,
			      GBool noClip, GBool nonIsolated) {
  SplashPipe pipe;
  SplashColor pixel;
  Guchar alpha;
  Guchar *ap;
  int x, y;

  if (src->mode != bitmap->mode) {
    return splashErrModeMismatch;
  }

  if (src->alpha) {
    pipeInit(&pipe, xDest, yDest, NULL, pixel, state->fillAlpha,
	     gTrue, nonIsolated);
    for (y = 0; y < h; ++y) {
      pipeSetXY(&pipe, xDest, yDest + y);
      ap = src->getAlphaPtr() + (ySrc + y) * src->getWidth() + xSrc;
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	alpha = *ap++;
	if (noClip || state->clip->test(xDest + x, yDest + y)) {
	  pipe.shape = (SplashCoord)(alpha / 255.0);
	  pipeRun(&pipe);
	  updateModX(xDest + x);
	  updateModY(yDest + y);
	} else {
	  pipeIncX(&pipe);
	}
      }
    }
  } else {
    pipeInit(&pipe, xDest, yDest, NULL, pixel, state->fillAlpha,
	     gFalse, nonIsolated);
    for (y = 0; y < h; ++y) {
      pipeSetXY(&pipe, xDest, yDest + y);
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	if (noClip || state->clip->test(xDest + x, yDest + y)) {
	  pipeRun(&pipe);
	  updateModX(xDest + x);
	  updateModY(yDest + y);
	} else {
	  pipeIncX(&pipe);
	}
      }
    }
  }

  return splashOk;
}
