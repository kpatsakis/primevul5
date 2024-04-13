void GfxGouraudTriangleShading::getTriangle(
				    int i,
				    double *x0, double *y0, GfxColor *color0,
				    double *x1, double *y1, GfxColor *color1,
				    double *x2, double *y2, GfxColor *color2) {
  double in;
  double out[gfxColorMaxComps];
  int v, j;

  v = triangles[i][0];
  *x0 = vertices[v].x;
  *y0 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < gfxColorMaxComps; ++j) {
      color0->c[j] = dblToCol(out[j]);
    }
  } else {
    *color0 = vertices[v].color;
  }
  v = triangles[i][1];
  *x1 = vertices[v].x;
  *y1 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < gfxColorMaxComps; ++j) {
      color1->c[j] = dblToCol(out[j]);
    }
  } else {
    *color1 = vertices[v].color;
  }
  v = triangles[i][2];
  *x2 = vertices[v].x;
  *y2 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < gfxColorMaxComps; ++j) {
      color2->c[j] = dblToCol(out[j]);
    }
  } else {
    *color2 = vertices[v].color;
  }
}
