inline void Splash::drawSpan(SplashPipe *pipe, int x0, int x1, int y,
			     GBool noClip) {
  int x;

  pipeSetXY(pipe, x0, y);
  if (noClip) {
    for (x = x0; x <= x1; ++x) {
      pipeRun(pipe);
    }
    updateModX(x0);
    updateModX(x1);
    updateModY(y);
  } else {
    for (x = x0; x <= x1; ++x) {
      if (state->clip->test(x, y)) {
	pipeRun(pipe);
	updateModX(x);
	updateModY(y);
      } else {
	pipeIncX(pipe);
      }
    }
  }
}
