GfxColorSpace *GfxIndexedColorSpace::parse(Array *arr) {
  GfxIndexedColorSpace *cs;
  GfxColorSpace *baseA;
  int indexHighA;
  Object obj1;
  int x;
  char *s;
  int n, i, j;

  if (arr->getLength() != 4) {
    error(-1, "Bad Indexed color space");
    goto err1;
  }
  arr->get(1, &obj1);
  if (!(baseA = GfxColorSpace::parse(&obj1))) {
    error(-1, "Bad Indexed color space (base color space)");
    goto err2;
  }
  obj1.free();
  if (!arr->get(2, &obj1)->isInt()) {
    error(-1, "Bad Indexed color space (hival)");
    delete baseA;
    goto err2;
  }
  indexHighA = obj1.getInt();
  if (indexHighA < 0 || indexHighA > 255) {
    int previousValue = indexHighA;
    if (indexHighA < 0) indexHighA = 0;
    else indexHighA = 255;
    error(-1, "Bad Indexed color space (invalid indexHigh value, was %d using %d to try to recover)", previousValue, indexHighA);
  }
  obj1.free();
  cs = new GfxIndexedColorSpace(baseA, indexHighA);
  arr->get(3, &obj1);
  n = baseA->getNComps();
  if (obj1.isStream()) {
    obj1.streamReset();
    for (i = 0; i <= indexHighA; ++i) {
      for (j = 0; j < n; ++j) {
	if ((x = obj1.streamGetChar()) == EOF) {
	  error(-1, "Bad Indexed color space (lookup table stream too short) padding with zeroes");
	  x = 0;
	}
	cs->lookup[i*n + j] = (Guchar)x;
      }
    }
    obj1.streamClose();
  } else if (obj1.isString()) {
    if (obj1.getString()->getLength() < (indexHighA + 1) * n) {
      error(-1, "Bad Indexed color space (lookup table string too short)");
      goto err3;
    }
    s = obj1.getString()->getCString();
    for (i = 0; i <= indexHighA; ++i) {
      for (j = 0; j < n; ++j) {
	cs->lookup[i*n + j] = (Guchar)*s++;
      }
    }
  } else {
    error(-1, "Bad Indexed color space (lookup table)");
    goto err3;
  }
  obj1.free();
  return cs;

 err3:
  delete cs;
 err2:
  obj1.free();
 err1:
  return NULL;
}
