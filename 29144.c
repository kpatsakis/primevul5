GfxColorSpace *GfxColorSpace::parse(Object *csObj) {
  GfxColorSpace *cs;
  Object obj1;

  cs = NULL;
  if (csObj->isName()) {
    if (csObj->isName("DeviceGray") || csObj->isName("G")) {
      cs = new GfxDeviceGrayColorSpace();
    } else if (csObj->isName("DeviceRGB") || csObj->isName("RGB")) {
      cs = new GfxDeviceRGBColorSpace();
    } else if (csObj->isName("DeviceCMYK") || csObj->isName("CMYK")) {
      cs = new GfxDeviceCMYKColorSpace();
    } else if (csObj->isName("Pattern")) {
      cs = new GfxPatternColorSpace(NULL);
    } else {
      error(-1, "Bad color space '%s'", csObj->getName());
    }
  } else if (csObj->isArray()) {
    csObj->arrayGet(0, &obj1);
    if (obj1.isName("DeviceGray") || obj1.isName("G")) {
      cs = new GfxDeviceGrayColorSpace();
    } else if (obj1.isName("DeviceRGB") || obj1.isName("RGB")) {
      cs = new GfxDeviceRGBColorSpace();
    } else if (obj1.isName("DeviceCMYK") || obj1.isName("CMYK")) {
      cs = new GfxDeviceCMYKColorSpace();
    } else if (obj1.isName("CalGray")) {
      cs = GfxCalGrayColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("CalRGB")) {
      cs = GfxCalRGBColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("Lab")) {
      cs = GfxLabColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("ICCBased")) {
      cs = GfxICCBasedColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("Indexed") || obj1.isName("I")) {
      cs = GfxIndexedColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("Separation")) {
      cs = GfxSeparationColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("DeviceN")) {
      cs = GfxDeviceNColorSpace::parse(csObj->getArray());
    } else if (obj1.isName("Pattern")) {
      cs = GfxPatternColorSpace::parse(csObj->getArray());
    } else {
      error(-1, "Bad color space");
    }
    obj1.free();
  } else {
    error(-1, "Bad color space - expected name or array");
  }
  return cs;
}
