void GfxPath::offset(double dx, double dy) {
  int i;

  for (i = 0; i < n; ++i) {
    subpaths[i]->offset(dx, dy);
  }
}
