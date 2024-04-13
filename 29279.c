SplashError Splash::fillWithPattern(SplashPath *path, GBool eo,
				    SplashPattern *pattern,
				    SplashCoord alpha) {
  SplashPipe pipe;
  SplashXPath *xPath;
  SplashXPathScanner *scanner;
  int xMinI, yMinI, xMaxI, yMaxI, x0, x1, y;
  SplashClipResult clipRes, clipRes2;

  if (path->length == 0) {
    return splashErrEmptyPath;
  }
  xPath = new SplashXPath(path, state->matrix, state->flatness, gTrue);
  if (vectorAntialias) {
    xPath->aaScale();
  }
  xPath->sort();
  scanner = new SplashXPathScanner(xPath, eo);

  if (vectorAntialias) {
    scanner->getBBoxAA(&xMinI, &yMinI, &xMaxI, &yMaxI);
  } else {
    scanner->getBBox(&xMinI, &yMinI, &xMaxI, &yMaxI);
  }

  if ((clipRes = state->clip->testRect(xMinI, yMinI, xMaxI, yMaxI))
      != splashClipAllOutside) {

    if (yMinI < state->clip->getYMinI()) {
      yMinI = state->clip->getYMinI();
    }
    if (yMaxI > state->clip->getYMaxI()) {
      yMaxI = state->clip->getYMaxI();
    }

    pipeInit(&pipe, 0, yMinI, pattern, NULL, alpha, vectorAntialias, gFalse);

    if (vectorAntialias) {
      for (y = yMinI; y <= yMaxI; ++y) {
	scanner->renderAALine(aaBuf, &x0, &x1, y);
	if (clipRes != splashClipAllInside) {
	  state->clip->clipAALine(aaBuf, &x0, &x1, y);
	}
	drawAALine(&pipe, x0, x1, y);
      }
    } else {
      for (y = yMinI; y <= yMaxI; ++y) {
	while (scanner->getNextSpan(y, &x0, &x1)) {
	  if (clipRes == splashClipAllInside) {
	    drawSpan(&pipe, x0, x1, y, gTrue);
	  } else {
	    if (x0 < state->clip->getXMinI()) {
	      x0 = state->clip->getXMinI();
	    }
	    if (x1 > state->clip->getXMaxI()) {
	      x1 = state->clip->getXMaxI();
	    }
	    clipRes2 = state->clip->testSpan(x0, x1, y);
	    drawSpan(&pipe, x0, x1, y, clipRes2 == splashClipAllInside);
	  }
	}
      }
    }
  }
  opClipRes = clipRes;

  delete scanner;
  delete xPath;
  return splashOk;
}
