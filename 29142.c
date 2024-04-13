void GfxSubpath::offset(double dx, double dy) {
  int i;

  for (i = 0; i < n; ++i) {
    x[i] += dx;
    y[i] += dy;
  }
}
