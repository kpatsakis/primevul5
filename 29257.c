GBool PSOutputDev::axialShadedFill(GfxState *state, GfxAxialShading *shading) {
  double xMin, yMin, xMax, yMax;
  double x0, y0, x1, y1, dx, dy, mul;
  double tMin, tMax, t, t0, t1;
  int i;

  if (level == psLevel2Sep || level == psLevel3Sep) {
    if (shading->getColorSpace()->getMode() != csDeviceCMYK) {
      return gFalse;
    }
    processColors |= psProcessCMYK;
  }

  state->getUserClipBBox(&xMin, &yMin, &xMax, &yMax);

  shading->getCoords(&x0, &y0, &x1, &y1);
  dx = x1 - x0;
  dy = y1 - y0;
  if (fabs(dx) < 0.01 && fabs(dy) < 0.01) {
    return gTrue;
  } else {
    mul = 1 / (dx * dx + dy * dy);
    tMin = tMax = ((xMin - x0) * dx + (yMin - y0) * dy) * mul;
    t = ((xMin - x0) * dx + (yMax - y0) * dy) * mul;
    if (t < tMin) {
      tMin = t;
    } else if (t > tMax) {
      tMax = t;
    }
    t = ((xMax - x0) * dx + (yMin - y0) * dy) * mul;
    if (t < tMin) {
      tMin = t;
    } else if (t > tMax) {
      tMax = t;
    }
    t = ((xMax - x0) * dx + (yMax - y0) * dy) * mul;
    if (t < tMin) {
      tMin = t;
    } else if (t > tMax) {
      tMax = t;
    }
    if (tMin < 0 && !shading->getExtend0()) {
      tMin = 0;
    }
    if (tMax > 1 && !shading->getExtend1()) {
      tMax = 1;
    }
  }

  t0 = shading->getDomain0();
  t1 = shading->getDomain1();

  writePSFmt("/t0 {0:.4g} def\n", t0);
  writePSFmt("/t1 {0:.4g} def\n", t1);
  writePSFmt("/dt {0:.4g} def\n", t1 - t0);
  writePSFmt("/x0 {0:.4g} def\n", x0);
  writePSFmt("/y0 {0:.4g} def\n", y0);
  writePSFmt("/dx {0:.4g} def\n", x1 - x0);
  writePSFmt("/x1 {0:.4g} def\n", x1);
  writePSFmt("/y1 {0:.4g} def\n", y1);
  writePSFmt("/dy {0:.4g} def\n", y1 - y0);
  writePSFmt("/xMin {0:.4g} def\n", xMin);
  writePSFmt("/yMin {0:.4g} def\n", yMin);
  writePSFmt("/xMax {0:.4g} def\n", xMax);
  writePSFmt("/yMax {0:.4g} def\n", yMax);
  writePSFmt("/n {0:d} def\n", shading->getColorSpace()->getNComps());
  if (shading->getNFuncs() == 1) {
    writePS("/func ");
    cvtFunction(shading->getFunc(0));
    writePS("def\n");
  } else {
    writePS("/func {\n");
    for (i = 0; i < shading->getNFuncs(); ++i) {
      if (i < shading->getNFuncs() - 1) {
	writePS("dup\n");
      }
      cvtFunction(shading->getFunc(i));
      writePS("exec\n");
      if (i < shading->getNFuncs() - 1) {
	writePS("exch\n");
      }
    }
    writePS("} def\n");
  }
  writePSFmt("{0:.4g} {1:.4g} 0 axialSH\n", tMin, tMax);

  return gTrue;
}
