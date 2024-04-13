void SplashOutputDev::endTransparencyGroup(GfxState *state) {
  double *ctm;

  delete splash;
  bitmap = transpGroupStack->origBitmap;
  splash = transpGroupStack->origSplash;
  ctm = state->getCTM();
  state->shiftCTM(transpGroupStack->tx, transpGroupStack->ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
}
