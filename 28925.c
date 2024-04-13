void SplashOutputDev::eoClip(GfxState *state) {
  SplashPath *path;

  path = convertPath(state, state->getPath());
  splash->clipToPath(path, gTrue);
  delete path;
}
