GfxShading *GfxShading::parse(Object *obj) {
  GfxShading *shading;
  Dict *dict;
  int typeA;
  Object obj1;

  if (obj->isDict()) {
    dict = obj->getDict();
  } else if (obj->isStream()) {
    dict = obj->streamGetDict();
  } else {
    return NULL;
  }

  if (!dict->lookup("ShadingType", &obj1)->isInt()) {
    error(-1, "Invalid ShadingType in shading dictionary");
    obj1.free();
    return NULL;
  }
  typeA = obj1.getInt();
  obj1.free();

  switch (typeA) {
  case 1:
    shading = GfxFunctionShading::parse(dict);
    break;
  case 2:
    shading = GfxAxialShading::parse(dict);
    break;
  case 3:
    shading = GfxRadialShading::parse(dict);
    break;
  case 4:
    if (obj->isStream()) {
      shading = GfxGouraudTriangleShading::parse(4, dict, obj->getStream());
    } else {
      error(-1, "Invalid Type 4 shading object");
      goto err1;
    }
    break;
  case 5:
    if (obj->isStream()) {
      shading = GfxGouraudTriangleShading::parse(5, dict, obj->getStream());
    } else {
      error(-1, "Invalid Type 5 shading object");
      goto err1;
    }
    break;
  case 6:
    if (obj->isStream()) {
      shading = GfxPatchMeshShading::parse(6, dict, obj->getStream());
    } else {
      error(-1, "Invalid Type 6 shading object");
      goto err1;
    }
    break;
  case 7:
    if (obj->isStream()) {
      shading = GfxPatchMeshShading::parse(7, dict, obj->getStream());
    } else {
      error(-1, "Invalid Type 7 shading object");
      goto err1;
    }
    break;
  default:
    error(-1, "Unimplemented shading type %d", typeA);
    goto err1;
  }

  return shading;

 err1:
  return NULL;
}
