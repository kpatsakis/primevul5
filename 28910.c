void SplashOutputDev::clipToStrokePath(GfxState *state) {
  SplashPath *path, *path2;

  path = convertPath(state, state->getPath());
  path2 = splash->makeStrokePath(path);
  delete path;
  splash->clipToPath(path2, gFalse);
  delete path2;
}
