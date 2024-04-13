void Splash::strokeWide(SplashPath *path) {
  SplashPath *path2;

  path2 = makeStrokePath(path, gFalse);
  fillWithPattern(path2, gFalse, state->strokePattern, state->strokeAlpha);
  delete path2;
}
