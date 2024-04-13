void Splash::dumpXPath(SplashXPath *path) {
  int i;

  for (i = 0; i < path->length; ++i) {
    printf("  %4d: x0=%8.2f y0=%8.2f x1=%8.2f y1=%8.2f %s%s%s%s%s%s%s\n",
	   i, (double)path->segs[i].x0, (double)path->segs[i].y0,
	   (double)path->segs[i].x1, (double)path->segs[i].y1,
	   (path->segs[i].flags	& splashXPathFirst) ? "F" : " ",
	   (path->segs[i].flags	& splashXPathLast) ? "L" : " ",
	   (path->segs[i].flags	& splashXPathEnd0) ? "0" : " ",
	   (path->segs[i].flags	& splashXPathEnd1) ? "1" : " ",
	   (path->segs[i].flags	& splashXPathHoriz) ? "H" : " ",
	   (path->segs[i].flags	& splashXPathVert) ? "V" : " ",
	   (path->segs[i].flags	& splashXPathFlip) ? "P" : " ");
  }
}
