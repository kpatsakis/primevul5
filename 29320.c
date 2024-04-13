void Splash::strokeNarrow(SplashPath *path) {
  SplashPipe pipe;
  SplashXPath *xPath;
  SplashXPathSeg *seg;
  int x0, x1, x2, x3, y0, y1, x, y, t;
  SplashCoord dx, dy, dxdy;
  SplashClipResult clipRes;
  int nClipRes[3];
  int i;

  nClipRes[0] = nClipRes[1] = nClipRes[2] = 0;

  xPath = new SplashXPath(path, state->matrix, state->flatness, gFalse);

  pipeInit(&pipe, 0, 0, state->strokePattern, NULL, state->strokeAlpha,
	   gFalse, gFalse);

  for (i = 0, seg = xPath->segs; i < xPath->length; ++i, ++seg) {

    x0 = splashFloor(seg->x0);
    x1 = splashFloor(seg->x1);
    y0 = splashFloor(seg->y0);
    y1 = splashFloor(seg->y1);

    if (y0 == y1) {
      if (x0 > x1) {
	t = x0; x0 = x1; x1 = t;
      }
      if ((clipRes = state->clip->testSpan(x0, x1, y0))
	  != splashClipAllOutside) {
	drawSpan(&pipe, x0, x1, y0, clipRes == splashClipAllInside);
      }

    } else if (splashAbs(seg->dxdy) > 1) {
      dx = seg->x1 - seg->x0;
      dy = seg->y1 - seg->y0;
      dxdy = seg->dxdy;
      if (y0 > y1) {
	t = y0; y0 = y1; y1 = t;
	t = x0; x0 = x1; x1 = t;
	dx = -dx;
	dy = -dy;
      }
      if ((clipRes = state->clip->testRect(x0 <= x1 ? x0 : x1, y0,
					   x0 <= x1 ? x1 : x0, y1))
	  != splashClipAllOutside) {
	if (dx > 0) {
	  x2 = x0;
	  x3 = splashFloor(seg->x0 + ((SplashCoord)y0 + 1 - seg->y0) * dxdy);
	  drawSpan(&pipe, x2, (x2 <= x3 - 1) ? x3 - 1 : x2, y0,
		   clipRes == splashClipAllInside);
	  x2 = x3;
	  for (y = y0 + 1; y <= y1 - 1; ++y) {
	    x3 = splashFloor(seg->x0 + ((SplashCoord)y + 1 - seg->y0) * dxdy);
	    drawSpan(&pipe, x2, x3 - 1, y, clipRes == splashClipAllInside);
	    x2 = x3;
	  }
	  drawSpan(&pipe, x2, x2 <= x1 ? x1 : x2, y1,
		   clipRes == splashClipAllInside);
	} else {
	  x2 = x0;
	  x3 = splashFloor(seg->x0 + ((SplashCoord)y0 + 1 - seg->y0) * dxdy);
	  drawSpan(&pipe, (x3 + 1 <= x2) ? x3 + 1 : x2, x2, y0,
		   clipRes == splashClipAllInside);
	  x2 = x3;
	  for (y = y0 + 1; y <= y1 - 1; ++y) {
	    x3 = splashFloor(seg->x0 + ((SplashCoord)y + 1 - seg->y0) * dxdy);
	    drawSpan(&pipe, x3 + 1, x2, y, clipRes == splashClipAllInside);
	    x2 = x3;
	  }
	  drawSpan(&pipe, x1, (x1 <= x2) ? x2 : x1, y1,
		   clipRes == splashClipAllInside);
	}
      }

    } else {
      dxdy = seg->dxdy;
      if (y0 > y1) {
	t = x0; x0 = x1; x1 = t;
	t = y0; y0 = y1; y1 = t;
      }
      if ((clipRes = state->clip->testRect(x0 <= x1 ? x0 : x1, y0,
					   x0 <= x1 ? x1 : x0, y1))
	  != splashClipAllOutside) {
	drawPixel(&pipe, x0, y0, clipRes == splashClipAllInside);
	for (y = y0 + 1; y <= y1 - 1; ++y) {
	  x = splashFloor(seg->x0 + ((SplashCoord)y - seg->y0) * dxdy);
	  drawPixel(&pipe, x, y, clipRes == splashClipAllInside);
	}
	drawPixel(&pipe, x1, y1, clipRes == splashClipAllInside);
    }
    }
    ++nClipRes[clipRes];
  }
  if (nClipRes[splashClipPartial] ||
      (nClipRes[splashClipAllInside] && nClipRes[splashClipAllOutside])) {
    opClipRes = splashClipPartial;
  } else if (nClipRes[splashClipAllInside]) {
    opClipRes = splashClipAllInside;
  } else {
    opClipRes = splashClipAllOutside;
  }

  delete xPath;
}
