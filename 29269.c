inline void Splash::drawAALine(SplashPipe *pipe, int x0, int x1, int y) {
#if splashAASize == 4
  static int bitCount4[16] = { 0, 1, 1, 2, 1, 2, 2, 3,
			       1, 2, 2, 3, 2, 3, 3, 4 };
  SplashColorPtr p0, p1, p2, p3;
  int t;
#else
  SplashColorPtr p;
  int xx, yy, t;
#endif
  int x;

#if splashAASize == 4
  p0 = aaBuf->getDataPtr() + (x0 >> 1);
  p1 = p0 + aaBuf->getRowSize();
  p2 = p1 + aaBuf->getRowSize();
  p3 = p2 + aaBuf->getRowSize();
#endif
  pipeSetXY(pipe, x0, y);
  for (x = x0; x <= x1; ++x) {

#if splashAASize == 4
    if (x & 1) {
      t = bitCount4[*p0 & 0x0f] + bitCount4[*p1 & 0x0f] +
	  bitCount4[*p2 & 0x0f] + bitCount4[*p3 & 0x0f];
      ++p0; ++p1; ++p2; ++p3;
    } else {
      t = bitCount4[*p0 >> 4] + bitCount4[*p1 >> 4] +
	  bitCount4[*p2 >> 4] + bitCount4[*p3 >> 4];
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
      pipe->shape = aaGamma[t];
      pipeRun(pipe);
      updateModX(x);
      updateModY(y);
    } else {
      pipeIncX(pipe);
    }
  }
}
