void GfxState::clipToStrokePath() {
  double xMin, yMin, xMax, yMax, x, y, t0, t1;
  GfxSubpath *subpath;
  int i, j;

  xMin = xMax = yMin = yMax = 0; // make gcc happy
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    for (j = 0; j < subpath->getNumPoints(); ++j) {
      transform(subpath->getX(j), subpath->getY(j), &x, &y);
      if (i == 0 && j == 0) {
	xMin = xMax = x;
	yMin = yMax = y;
      } else {
	if (x < xMin) {
	  xMin = x;
	} else if (x > xMax) {
	  xMax = x;
	}
	if (y < yMin) {
	  yMin = y;
	} else if (y > yMax) {
	  yMax = y;
	}
      }
    }
  }

  t0 = fabs(ctm[0]);
  t1 = fabs(ctm[2]);
  if (t0 > t1) {
    xMin -= 0.5 * lineWidth * t0;
    xMax += 0.5 * lineWidth * t0;
  } else {
    xMin -= 0.5 * lineWidth * t1;
    xMax += 0.5 * lineWidth * t1;
  }
  t0 = fabs(ctm[0]);
  t1 = fabs(ctm[3]);
  if (t0 > t1) {
    yMin -= 0.5 * lineWidth * t0;
    yMax += 0.5 * lineWidth * t0;
  } else {
    yMin -= 0.5 * lineWidth * t1;
    yMax += 0.5 * lineWidth * t1;
  }

  if (xMin > clipXMin) {
    clipXMin = xMin;
  }
  if (yMin > clipYMin) {
    clipYMin = yMin;
  }
  if (xMax < clipXMax) {
    clipXMax = xMax;
  }
  if (yMax < clipYMax) {
    clipYMax = yMax;
  }
}
