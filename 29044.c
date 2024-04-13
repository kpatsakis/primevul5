void GfxPath::append(GfxPath *path) {
  int i;

  if (n + path->n > size) {
    size = n + path->n;
    subpaths = (GfxSubpath **)
                 greallocn(subpaths, size, sizeof(GfxSubpath *));
  }
  for (i = 0; i < path->n; ++i) {
    subpaths[n++] = path->subpaths[i]->copy();
  }
  justMoved = gFalse;
}
