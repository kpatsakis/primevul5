static int glyphPathMoveTo(const FT_Vector *pt, void *path) {
  SplashFTFontPath *p = (SplashFTFontPath *)path;

  if (p->needClose) {
    p->path->close();
    p->needClose = gFalse;
  }
  p->path->moveTo((SplashCoord)pt->x * p->textScale / 64.0,
		  (SplashCoord)pt->y * p->textScale / 64.0);
  return 0;
}
