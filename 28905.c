void SplashOutputDev::beginTransparencyGroup(GfxState *state, double *bbox,
					     GfxColorSpace *blendingColorSpace,
					     GBool isolated, GBool /*knockout*/,
					     GBool /*forSoftMask*/) {
  SplashTransparencyGroup *transpGroup;
  SplashColor color;
  double xMin, yMin, xMax, yMax, x, y;
  int tx, ty, w, h;

  state->transform(bbox[0], bbox[1], &x, &y);
  xMin = xMax = x;
  yMin = yMax = y;
  state->transform(bbox[0], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[1], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  tx = (int)floor(xMin);
  if (tx < 0) {
    tx = 0;
  } else if (tx > bitmap->getWidth()) {
    tx = bitmap->getWidth();
  }
  ty = (int)floor(yMin);
  if (ty < 0) {
    ty = 0;
  } else if (ty > bitmap->getHeight()) {
    ty = bitmap->getHeight();
  }
  w = (int)ceil(xMax) - tx + 1;
  if (tx + w > bitmap->getWidth()) {
    w = bitmap->getWidth() - tx;
  }
  if (w < 1) {
    w = 1;
  }
  h = (int)ceil(yMax) - ty + 1;
  if (ty + h > bitmap->getHeight()) {
    h = bitmap->getHeight() - ty;
  }
  if (h < 1) {
    h = 1;
  }

  transpGroup = new SplashTransparencyGroup();
  transpGroup->tx = tx;
  transpGroup->ty = ty;
  transpGroup->blendingColorSpace = blendingColorSpace;
  transpGroup->isolated = isolated;
  transpGroup->next = transpGroupStack;
  transpGroupStack = transpGroup;

  transpGroup->origBitmap = bitmap;
  transpGroup->origSplash = splash;


  bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode, gTrue,
			    bitmapTopDown); 
  splash = new Splash(bitmap, vectorAntialias,
		      transpGroup->origSplash->getScreen());
  if (isolated) {
    switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      color[0] = 0;
      break;
    case splashModeXBGR8:
      color[3] = 255;
    case splashModeRGB8:
    case splashModeBGR8:
      color[0] = color[1] = color[2] = 0;
      break;
#if SPLASH_CMYK
    case splashModeCMYK8:
      color[0] = color[1] = color[2] = color[3] = 0;
      break;
#endif
    default:
      break;
    }
    splash->clear(color, 0);
  } else {
    splash->blitTransparent(transpGroup->origBitmap, tx, ty, 0, 0, w, h);
    splash->setInNonIsolatedGroup(transpGroup->origBitmap, tx, ty);
  }
  transpGroup->tBitmap = bitmap;
  state->shiftCTM(-tx, -ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
}
