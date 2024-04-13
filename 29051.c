void GfxState::concatCTM(double a, double b, double c,
			 double d, double e, double f) {
  double a1 = ctm[0];
  double b1 = ctm[1];
  double c1 = ctm[2];
  double d1 = ctm[3];
  int i;

  ctm[0] = a * a1 + b * c1;
  ctm[1] = a * b1 + b * d1;
  ctm[2] = c * a1 + d * c1;
  ctm[3] = c * b1 + d * d1;
  ctm[4] = e * a1 + f * c1 + ctm[4];
  ctm[5] = e * b1 + f * d1 + ctm[5];

  for (i = 0; i < 6; ++i) {
    if (ctm[i] > 1e10) {
      ctm[i] = 1e10;
    } else if (ctm[i] < -1e10) {
      ctm[i] = -1e10;
    }
  }
}
