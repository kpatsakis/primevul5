SplashError Splash::blitTransparent(SplashBitmap *src, int xSrc, int ySrc,
				    int xDest, int yDest, int w, int h) {
  SplashColor pixel;
  SplashColorPtr p;
  Guchar *q;
  int x, y, mask;

  if (src->mode != bitmap->mode) {
    return splashErrModeMismatch;
  }

  switch (bitmap->mode) {
  case splashModeMono1:
    for (y = 0; y < h; ++y) {
      p = &bitmap->data[(yDest + y) * bitmap->rowSize + (xDest >> 3)];
      mask = 0x80 >> (xDest & 7);
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	if (pixel[0]) {
	  *p |= mask;
	} else {
	  *p &= ~mask;
	}
	if (!(mask >>= 1)) {
	  mask = 0x80;
	  ++p;
	}
      }
    }
    break;
  case splashModeMono8:
    for (y = 0; y < h; ++y) {
      p = &bitmap->data[(yDest + y) * bitmap->rowSize + xDest];
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	*p++ = pixel[0];
      }
    }
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    for (y = 0; y < h; ++y) {
      p = &bitmap->data[(yDest + y) * bitmap->rowSize + 3 * xDest];
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	*p++ = pixel[0];
	*p++ = pixel[1];
	*p++ = pixel[2];
      }
    }
    break;
  case splashModeXBGR8:
    for (y = 0; y < h; ++y) {
      p = &bitmap->data[(yDest + y) * bitmap->rowSize + 4 * xDest];
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	*p++ = pixel[0];
	*p++ = pixel[1];
	*p++ = pixel[2];
	*p++ = 255;
      }
    }
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    for (y = 0; y < h; ++y) {
      p = &bitmap->data[(yDest + y) * bitmap->rowSize + 4 * xDest];
      for (x = 0; x < w; ++x) {
	src->getPixel(xSrc + x, ySrc + y, pixel);
	*p++ = pixel[0];
	*p++ = pixel[1];
	*p++ = pixel[2];
	*p++ = pixel[3];
      }
    }
    break;
#endif
  }

  if (bitmap->alpha) {
    for (y = 0; y < h; ++y) {
      q = &bitmap->alpha[(yDest + y) * bitmap->width + xDest];
      for (x = 0; x < w; ++x) {
	*q++ = 0x00;
      }
    }
  }

  return splashOk;
}
