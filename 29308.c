void Splash::setInNonIsolatedGroup(SplashBitmap *alpha0BitmapA,
				   int alpha0XA, int alpha0YA) {
  alpha0Bitmap = alpha0BitmapA;
  alpha0X = alpha0XA;
  alpha0Y = alpha0YA;
  state->inNonIsolatedGroup = gTrue;
}
