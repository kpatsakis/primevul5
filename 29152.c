GfxPattern *GfxPattern::parse(Object *obj) {
  GfxPattern *pattern;
  Object obj1;

  if (obj->isDict()) {
    obj->dictLookup("PatternType", &obj1);
  } else if (obj->isStream()) {
    obj->streamGetDict()->lookup("PatternType", &obj1);
  } else {
    return NULL;
  }
  pattern = NULL;
  if (obj1.isInt() && obj1.getInt() == 1) {
    pattern = GfxTilingPattern::parse(obj);
  } else if (obj1.isInt() && obj1.getInt() == 2) {
    pattern = GfxShadingPattern::parse(obj);
  }
  obj1.free();
  return pattern;
}
