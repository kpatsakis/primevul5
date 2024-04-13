static int glyphPathLineTo(const FT_Vector *pt, void *path) {
  SplashFTFontPath *p = (SplashFTFontPath *)path;

  p->path->lineTo((SplashCoord)pt->x * p->textScale / 64.0,
		  (SplashCoord)pt->y * p->textScale / 64.0);
  p->needClose = gTrue;
  return 0;
}
