inline void Splash::drawPixel(SplashPipe *pipe, int x, int y, GBool noClip) {
  if (noClip || state->clip->test(x, y)) {
    pipeSetXY(pipe, x, y);
    pipeRun(pipe);
    updateModX(x);
    updateModY(y);
  }
}
