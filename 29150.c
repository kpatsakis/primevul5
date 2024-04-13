GfxColorSpace *GfxDeviceNColorSpace::parse(Array *arr) {
  GfxDeviceNColorSpace *cs;
  int nCompsA;
  GooString *namesA[gfxColorMaxComps];
  GfxColorSpace *altA;
  Function *funcA;
  Object obj1, obj2;
  int i;

  if (arr->getLength() != 4 && arr->getLength() != 5) {
    error(-1, "Bad DeviceN color space");
    goto err1;
  }
  if (!arr->get(1, &obj1)->isArray()) {
    error(-1, "Bad DeviceN color space (names)");
    goto err2;
  }
  nCompsA = obj1.arrayGetLength();
  if (nCompsA > gfxColorMaxComps) {
    error(-1, "DeviceN color space with too many (%d > %d) components",
	  nCompsA, gfxColorMaxComps);
    nCompsA = gfxColorMaxComps;
  }
  for (i = 0; i < nCompsA; ++i) {
    if (!obj1.arrayGet(i, &obj2)->isName()) {
      error(-1, "Bad DeviceN color space (names)");
      obj2.free();
      goto err2;
    }
    namesA[i] = new GooString(obj2.getName());
    obj2.free();
  }
  obj1.free();
  arr->get(2, &obj1);
  if (!(altA = GfxColorSpace::parse(&obj1))) {
    error(-1, "Bad DeviceN color space (alternate color space)");
    goto err3;
  }
  obj1.free();
  arr->get(3, &obj1);
  if (!(funcA = Function::parse(&obj1))) {
    goto err4;
  }
  obj1.free();
  cs = new GfxDeviceNColorSpace(nCompsA, altA, funcA);
  cs->nonMarking = gTrue;
  for (i = 0; i < nCompsA; ++i) {
    cs->names[i] = namesA[i];
    if (namesA[i]->cmp("None")) {
      cs->nonMarking = gFalse;
    }
  }
  return cs;

 err4:
  delete altA;
 err3:
  for (i = 0; i < nCompsA; ++i) {
    delete namesA[i];
  }
 err2:
  obj1.free();
 err1:
  return NULL;
}
