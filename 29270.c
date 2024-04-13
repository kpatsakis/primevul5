inline void Splash::drawAAPixel(SplashPipe *pipe, int x, int y) {
#if splashAASize == 4
  static int bitCount4[16] = { 0, 1, 1, 2, 1, 2, 2, 3,
			       1, 2, 2, 3, 2, 3, 3, 4 };
  int w;
#else
  int xx, yy;
#endif
  SplashColorPtr p;
  int x0, x1, t;

  if (x < 0 || x >= bitmap->width ||
      y < state->clip->getYMinI() || y > state->clip->getYMaxI()) {
    return;
  }

  if (y != aaBufY) {
    memset(aaBuf->getDataPtr(), 0xff,
	   aaBuf->getRowSize() * aaBuf->getHeight());
    x0 = 0;
    x1 = bitmap->width - 1;
    state->clip->clipAALine(aaBuf, &x0, &x1, y);
    aaBufY = y;
  }

#if splashAASize == 4
  p = aaBuf->getDataPtr() + (x >> 1);
  w = aaBuf->getRowSize();
  if (x & 1) {
    t = bitCount4[*p & 0x0f] + bitCount4[p[w] & 0x0f] +
        bitCount4[p[2*w] & 0x0f] + bitCount4[p[3*w] & 0x0f];
  } else {
    t = bitCount4[*p >> 4] + bitCount4[p[w] >> 4] +
        bitCount4[p[2*w] >> 4] + bitCount4[p[3*w] >> 4];
  }
#else
  t = 0;
  for (yy = 0; yy < splashAASize; ++yy) {
    for (xx = 0; xx < splashAASize; ++xx) {
      p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() +
	  ((x * splashAASize + xx) >> 3);
      t += (*p >> (7 - ((x * splashAASize + xx) & 7))) & 1;
    }
  }
#endif

  if (t != 0) {
    pipeSetXY(pipe, x, y);
    pipe->shape *= aaGamma[t];
    pipeRun(pipe);
    updateModX(x);
    updateModY(y);
  }
}
