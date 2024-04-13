GfxColorSpace *GfxSeparationColorSpace::parse(Array *arr) {
  GfxSeparationColorSpace *cs;
  GooString *nameA;
  GfxColorSpace *altA;
  Function *funcA;
  Object obj1;

  if (arr->getLength() != 4) {
    error(-1, "Bad Separation color space");
    goto err1;
  }
  if (!arr->get(1, &obj1)->isName()) {
    error(-1, "Bad Separation color space (name)");
    goto err2;
  }
  nameA = new GooString(obj1.getName());
  obj1.free();
  arr->get(2, &obj1);
  if (!(altA = GfxColorSpace::parse(&obj1))) {
    error(-1, "Bad Separation color space (alternate color space)");
    goto err3;
  }
  obj1.free();
  arr->get(3, &obj1);
  if (!(funcA = Function::parse(&obj1))) {
    goto err4;
  }
  obj1.free();
  cs = new GfxSeparationColorSpace(nameA, altA, funcA);
  return cs;

 err4:
  delete altA;
 err3:
  delete nameA;
 err2:
  obj1.free();
 err1:
  return NULL;
}
