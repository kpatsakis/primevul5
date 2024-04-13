SplashPath *Splash::makeStrokePath(SplashPath *path, GBool flatten) {
  SplashPath *pathIn, *pathOut;
  SplashCoord w, d, dx, dy, wdx, wdy, dxNext, dyNext, wdxNext, wdyNext;
  SplashCoord crossprod, dotprod, miter, m;
  GBool first, last, closed;
  int subpathStart, next, i;
  int left0, left1, left2, right0, right1, right2, join0, join1, join2;
  int leftFirst, rightFirst, firstPt;

  if (flatten) {
    pathIn = flattenPath(path, state->matrix, state->flatness);
    if (state->lineDashLength > 0) {
      pathOut = makeDashedPath(pathIn);
      delete pathIn;
      pathIn = pathOut;
    }
  } else {
    pathIn = path;
  }

  subpathStart = 0; // make gcc happy
  closed = gFalse; // make gcc happy
  left0 = left1 = right0 = right1 = join0 = join1 = 0; // make gcc happy
  leftFirst = rightFirst = firstPt = 0; // make gcc happy

  pathOut = new SplashPath();
  w = state->lineWidth;

  for (i = 0; i < pathIn->length - 1; ++i) {
    if (pathIn->flags[i] & splashPathLast) {
      continue;
    }
    if ((first = pathIn->flags[i] & splashPathFirst)) {
      subpathStart = i;
      closed = pathIn->flags[i] & splashPathClosed;
    }
    last = pathIn->flags[i+1] & splashPathLast;

    d = splashDist(pathIn->pts[i].x, pathIn->pts[i].y,
		   pathIn->pts[i+1].x, pathIn->pts[i+1].y);
    if (d == 0) {
      dx = 0;
      dy = 1;
    } else {
      d = (SplashCoord)1 / d;
      dx = d * (pathIn->pts[i+1].x - pathIn->pts[i].x);
      dy = d * (pathIn->pts[i+1].y - pathIn->pts[i].y);
    }
    wdx = (SplashCoord)0.5 * w * dx;
    wdy = (SplashCoord)0.5 * w * dy;

    next = last ? subpathStart + 1 : i + 2;
    d = splashDist(pathIn->pts[i+1].x, pathIn->pts[i+1].y,
		   pathIn->pts[next].x, pathIn->pts[next].y);
    if (d == 0) {
      dxNext = 0;
      dyNext = 1;
    } else {
      d = (SplashCoord)1 / d;
      dxNext = d * (pathIn->pts[next].x - pathIn->pts[i+1].x);
      dyNext = d * (pathIn->pts[next].y - pathIn->pts[i+1].y);
    }
    wdxNext = (SplashCoord)0.5 * w * dxNext;
    wdyNext = (SplashCoord)0.5 * w * dyNext;

    pathOut->moveTo(pathIn->pts[i].x - wdy, pathIn->pts[i].y + wdx);
    if (i == subpathStart) {
      firstPt = pathOut->length - 1;
    }
    if (first && !closed) {
      switch (state->lineCap) {
      case splashLineCapButt:
	pathOut->lineTo(pathIn->pts[i].x + wdy, pathIn->pts[i].y - wdx);
	break;
      case splashLineCapRound:
	pathOut->curveTo(pathIn->pts[i].x - wdy - bezierCircle * wdx,
			 pathIn->pts[i].y + wdx - bezierCircle * wdy,
			 pathIn->pts[i].x - wdx - bezierCircle * wdy,
			 pathIn->pts[i].y - wdy + bezierCircle * wdx,
			 pathIn->pts[i].x - wdx,
			 pathIn->pts[i].y - wdy);
	pathOut->curveTo(pathIn->pts[i].x - wdx + bezierCircle * wdy,
			 pathIn->pts[i].y - wdy - bezierCircle * wdx,
			 pathIn->pts[i].x + wdy - bezierCircle * wdx,
			 pathIn->pts[i].y - wdx - bezierCircle * wdy,
			 pathIn->pts[i].x + wdy,
			 pathIn->pts[i].y - wdx);
	break;
      case splashLineCapProjecting:
	pathOut->lineTo(pathIn->pts[i].x - wdx - wdy,
			pathIn->pts[i].y + wdx - wdy);
	pathOut->lineTo(pathIn->pts[i].x - wdx + wdy,
			pathIn->pts[i].y - wdx - wdy);
	pathOut->lineTo(pathIn->pts[i].x + wdy,
			pathIn->pts[i].y - wdx);
	break;
      }
    } else {
      pathOut->lineTo(pathIn->pts[i].x + wdy, pathIn->pts[i].y - wdx);
    }

    left2 = pathOut->length - 1;
    pathOut->lineTo(pathIn->pts[i+1].x + wdy, pathIn->pts[i+1].y - wdx);

    if (last && !closed) {
      switch (state->lineCap) {
      case splashLineCapButt:
	pathOut->lineTo(pathIn->pts[i+1].x - wdy, pathIn->pts[i+1].y + wdx);
	break;
      case splashLineCapRound:
	pathOut->curveTo(pathIn->pts[i+1].x + wdy + bezierCircle * wdx,
			 pathIn->pts[i+1].y - wdx + bezierCircle * wdy,
			 pathIn->pts[i+1].x + wdx + bezierCircle * wdy,
			 pathIn->pts[i+1].y + wdy - bezierCircle * wdx,
			 pathIn->pts[i+1].x + wdx,
			 pathIn->pts[i+1].y + wdy);
	pathOut->curveTo(pathIn->pts[i+1].x + wdx - bezierCircle * wdy,
			 pathIn->pts[i+1].y + wdy + bezierCircle * wdx,
			 pathIn->pts[i+1].x - wdy + bezierCircle * wdx,
			 pathIn->pts[i+1].y + wdx + bezierCircle * wdy,
			 pathIn->pts[i+1].x - wdy,
			 pathIn->pts[i+1].y + wdx);
	break;
      case splashLineCapProjecting:
	pathOut->lineTo(pathIn->pts[i+1].x + wdy + wdx,
			pathIn->pts[i+1].y - wdx + wdy);
	pathOut->lineTo(pathIn->pts[i+1].x - wdy + wdx,
			pathIn->pts[i+1].y + wdx + wdy);
	pathOut->lineTo(pathIn->pts[i+1].x - wdy,
			pathIn->pts[i+1].y + wdx);
	break;
      }
    } else {
      pathOut->lineTo(pathIn->pts[i+1].x - wdy, pathIn->pts[i+1].y + wdx);
    }

    right2 = pathOut->length - 1;
    pathOut->close();

    join2 = pathOut->length;
    if (!last || closed) {
      crossprod = dx * dyNext - dy * dxNext;
      dotprod = -(dx * dxNext + dy * dyNext);
      if (dotprod > 0.99999) {
	miter = (state->miterLimit + 1) * (state->miterLimit + 1);
	m = 0;
      } else {
	miter = (SplashCoord)2 / ((SplashCoord)1 - dotprod);
	if (miter < 1) {
	  miter = 1;
	}
	m = splashSqrt(miter - 1);
      }

      if (state->lineJoin == splashLineJoinRound) {
	pathOut->moveTo(pathIn->pts[i+1].x + (SplashCoord)0.5 * w,
			pathIn->pts[i+1].y);
	pathOut->curveTo(pathIn->pts[i+1].x + (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y + bezierCircle2 * w,
			 pathIn->pts[i+1].x + bezierCircle2 * w,
			 pathIn->pts[i+1].y + (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].x,
			 pathIn->pts[i+1].y + (SplashCoord)0.5 * w);
	pathOut->curveTo(pathIn->pts[i+1].x - bezierCircle2 * w,
			 pathIn->pts[i+1].y + (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].x - (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y + bezierCircle2 * w,
			 pathIn->pts[i+1].x - (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y);
	pathOut->curveTo(pathIn->pts[i+1].x - (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y - bezierCircle2 * w,
			 pathIn->pts[i+1].x - bezierCircle2 * w,
			 pathIn->pts[i+1].y - (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].x,
			 pathIn->pts[i+1].y - (SplashCoord)0.5 * w);
	pathOut->curveTo(pathIn->pts[i+1].x + bezierCircle2 * w,
			 pathIn->pts[i+1].y - (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].x + (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y - bezierCircle2 * w,
			 pathIn->pts[i+1].x + (SplashCoord)0.5 * w,
			 pathIn->pts[i+1].y);

      } else {
	pathOut->moveTo(pathIn->pts[i+1].x, pathIn->pts[i+1].y);

	if (crossprod < 0) {
	  pathOut->lineTo(pathIn->pts[i+1].x - wdyNext,
			  pathIn->pts[i+1].y + wdxNext);
	  if (state->lineJoin == splashLineJoinMiter &&
	      splashSqrt(miter) <= state->miterLimit) {
	    pathOut->lineTo(pathIn->pts[i+1].x - wdy + wdx * m,
			    pathIn->pts[i+1].y + wdx + wdy * m);
	    pathOut->lineTo(pathIn->pts[i+1].x - wdy,
			    pathIn->pts[i+1].y + wdx);
	  } else {
	    pathOut->lineTo(pathIn->pts[i+1].x - wdy, pathIn->pts[i+1].y + wdx);
	  }

	} else {
	  pathOut->lineTo(pathIn->pts[i+1].x + wdy,
			  pathIn->pts[i+1].y - wdx);
	  if (state->lineJoin == splashLineJoinMiter &&
	      splashSqrt(miter) <= state->miterLimit) {
	    pathOut->lineTo(pathIn->pts[i+1].x + wdy + wdx * m,
			    pathIn->pts[i+1].y - wdx + wdy * m);
	    pathOut->lineTo(pathIn->pts[i+1].x + wdyNext,
			    pathIn->pts[i+1].y - wdxNext);
	  } else {
	    pathOut->lineTo(pathIn->pts[i+1].x + wdyNext,
			    pathIn->pts[i+1].y - wdxNext);
	  }
	}
      }

      pathOut->close();
    }

    if (state->strokeAdjust) {
      if (i >= subpathStart + 1) {
	if (i >= subpathStart + 2) {
	  pathOut->addStrokeAdjustHint(left1, right1, left0 + 1, right0);
	  pathOut->addStrokeAdjustHint(left1, right1, join0, left2);
	} else {
	  pathOut->addStrokeAdjustHint(left1, right1, firstPt, left2);
	}
	pathOut->addStrokeAdjustHint(left1, right1, right2 + 1, right2 + 1);
      }
      left0 = left1;
      left1 = left2;
      right0 = right1;
      right1 = right2;
      join0 = join1;
      join1 = join2;
      if (i == subpathStart) {
	leftFirst = left2;
	rightFirst = right2;
      }
      if (last) {
	if (i >= subpathStart + 2) {
	  pathOut->addStrokeAdjustHint(left1, right1, left0 + 1, right0);
	  pathOut->addStrokeAdjustHint(left1, right1,
				       join0, pathOut->length - 1);
	} else {
	  pathOut->addStrokeAdjustHint(left1, right1,
				       firstPt, pathOut->length - 1);
	}
	if (closed) {
	  pathOut->addStrokeAdjustHint(left1, right1, firstPt, leftFirst);
	  pathOut->addStrokeAdjustHint(left1, right1,
				       rightFirst + 1, rightFirst + 1);
	  pathOut->addStrokeAdjustHint(leftFirst, rightFirst,
				       left1 + 1, right1);
	  pathOut->addStrokeAdjustHint(leftFirst, rightFirst,
				       join1, pathOut->length - 1);
	}
      }
    }
  }

  if (pathIn != path) {
    delete pathIn;
  }

  return pathOut;
}
