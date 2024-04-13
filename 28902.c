SplashOutputDev::SplashOutputDev(SplashColorMode colorModeA,
				 int bitmapRowPadA,
				 GBool reverseVideoA,
				 SplashColorPtr paperColorA,
				 GBool bitmapTopDownA,
				 GBool allowAntialiasA) {
  colorMode = colorModeA;
  bitmapRowPad = bitmapRowPadA;
  bitmapTopDown = bitmapTopDownA;
  allowAntialias = allowAntialiasA;
  vectorAntialias = allowAntialias &&
		      globalParams->getVectorAntialias() &&
		      colorMode != splashModeMono1;
  setupScreenParams(72.0, 72.0);
  reverseVideo = reverseVideoA;
  splashColorCopy(paperColor, paperColorA);

  xref = NULL;

  bitmap = new SplashBitmap(1, 1, bitmapRowPad, colorMode,
			    colorMode != splashModeMono1, bitmapTopDown);
  splash = new Splash(bitmap, vectorAntialias, &screenParams);
  splash->clear(paperColor, 0);

  fontEngine = NULL;

  nT3Fonts = 0;
  t3GlyphStack = NULL;

  font = NULL;
  needFontUpdate = gFalse;
  textClipPath = NULL;

  transpGroupStack = NULL;
}
