GBool GfxShading::init(Dict *dict) {
  Object obj1, obj2;
  int i;

  dict->lookup("ColorSpace", &obj1);
  if (!(colorSpace = GfxColorSpace::parse(&obj1))) {
    error(-1, "Bad color space in shading dictionary");
    obj1.free();
    return gFalse;
  }
  obj1.free();

  for (i = 0; i < gfxColorMaxComps; ++i) {
    background.c[i] = 0;
  }
  hasBackground = gFalse;
  if (dict->lookup("Background", &obj1)->isArray()) {
    if (obj1.arrayGetLength() == colorSpace->getNComps()) {
      hasBackground = gTrue;
      for (i = 0; i < colorSpace->getNComps(); ++i) {
	background.c[i] = dblToCol(obj1.arrayGet(i, &obj2)->getNum());
	obj2.free();
      }
    } else {
      error(-1, "Bad Background in shading dictionary");
    }
  }
  obj1.free();

  xMin = yMin = xMax = yMax = 0;
  hasBBox = gFalse;
  if (dict->lookup("BBox", &obj1)->isArray()) {
    if (obj1.arrayGetLength() == 4) {
      hasBBox = gTrue;
      xMin = obj1.arrayGet(0, &obj2)->getNum();
      obj2.free();
      yMin = obj1.arrayGet(1, &obj2)->getNum();
      obj2.free();
      xMax = obj1.arrayGet(2, &obj2)->getNum();
      obj2.free();
      yMax = obj1.arrayGet(3, &obj2)->getNum();
      obj2.free();
    } else {
      error(-1, "Bad BBox in shading dictionary");
    }
  }
  obj1.free();

  return gTrue;
}
