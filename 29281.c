SplashPath *Splash::flattenPath(SplashPath *path, SplashCoord *matrix,
				SplashCoord flatness) {
  SplashPath *fPath;
  SplashCoord flatness2;
  Guchar flag;
  int i;

  fPath = new SplashPath();
  flatness2 = flatness * flatness;
  i = 0;
  while (i < path->length) {
    flag = path->flags[i];
    if (flag & splashPathFirst) {
      fPath->moveTo(path->pts[i].x, path->pts[i].y);
      ++i;
    } else {
      if (flag & splashPathCurve) {
	flattenCurve(path->pts[i-1].x, path->pts[i-1].y,
		     path->pts[i  ].x, path->pts[i  ].y,
		     path->pts[i+1].x, path->pts[i+1].y,
		     path->pts[i+2].x, path->pts[i+2].y,
		     matrix, flatness2, fPath);
	i += 3;
      } else {
	fPath->lineTo(path->pts[i].x, path->pts[i].y);
	++i;
      }
      if (path->flags[i-1] & splashPathClosed) {
	fPath->close();
      }
    }
  }
  return fPath;
}
