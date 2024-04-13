static int glyphPathConicTo(const FT_Vector *ctrl, const FT_Vector *pt,
			    void *path) {
  SplashFTFontPath *p = (SplashFTFontPath *)path;
  SplashCoord x0, y0, x1, y1, x2, y2, x3, y3, xc, yc;

  if (!p->path->getCurPt(&x0, &y0)) {
    return 0;
  }
  xc = (SplashCoord)ctrl->x * p->textScale / 64.0;
  yc = (SplashCoord)ctrl->y * p->textScale / 64.0;
  x3 = (SplashCoord)pt->x * p->textScale / 64.0;
  y3 = (SplashCoord)pt->y * p->textScale / 64.0;


  x1 = (SplashCoord)(1.0 / 3.0) * (x0 + (SplashCoord)2 * xc);
  y1 = (SplashCoord)(1.0 / 3.0) * (y0 + (SplashCoord)2 * yc);
  x2 = (SplashCoord)(1.0 / 3.0) * ((SplashCoord)2 * xc + x3);
  y2 = (SplashCoord)(1.0 / 3.0) * ((SplashCoord)2 * yc + y3);

  p->path->curveTo(x1, y1, x2, y2, x3, y3);
  p->needClose = gTrue;
  return 0;
}
