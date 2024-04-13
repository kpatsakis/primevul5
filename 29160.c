GBool GfxState::parseBlendMode(Object *obj, GfxBlendMode *mode) {
  Object obj2;
  int i, j;

  if (obj->isName()) {
    for (i = 0; i < nGfxBlendModeNames; ++i) {
      if (!strcmp(obj->getName(), gfxBlendModeNames[i].name)) {
	*mode = gfxBlendModeNames[i].mode;
	return gTrue;
      }
    }
    return gFalse;
  } else if (obj->isArray()) {
    for (i = 0; i < obj->arrayGetLength(); ++i) {
      obj->arrayGet(i, &obj2);
      if (!obj2.isName()) {
	obj2.free();
	return gFalse;
      }
      for (j = 0; j < nGfxBlendModeNames; ++j) {
	if (!strcmp(obj2.getName(), gfxBlendModeNames[j].name)) {
	  obj2.free();
	  *mode = gfxBlendModeNames[j].mode;
	  return gTrue;
	}
      }
      obj2.free();
    }
    *mode = gfxBlendNormal;
    return gTrue;
  } else {
    return gFalse;
  }
}
