void GfxSubpath::lineTo(double x1, double y1) {
  if (n >= size) {
    size += 16;
    x = (double *)greallocn(x, size, sizeof(double));
    y = (double *)greallocn(y, size, sizeof(double));
    curve = (GBool *)greallocn(curve, size, sizeof(GBool));
  }
  x[n] = x1;
  y[n] = y1;
  curve[n] = gFalse;
  ++n;
}
