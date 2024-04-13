GfxColorSpace *GfxICCBasedColorSpace::parse(Array *arr) {
  GfxICCBasedColorSpace *cs;
  Ref iccProfileStreamA;
  int nCompsA;
  GfxColorSpace *altA;
  Dict *dict;
  Object obj1, obj2, obj3;
  int i;

  arr->getNF(1, &obj1);
  if (obj1.isRef()) {
    iccProfileStreamA = obj1.getRef();
  } else {
    iccProfileStreamA.num = 0;
    iccProfileStreamA.gen = 0;
  }
  obj1.free();
#ifdef USE_CMS
  if (iccProfileStreamA.num > 0
     && (cs = GfxICCBasedCache::lookup(iccProfileStreamA.num,
          iccProfileStreamA.gen)) != NULL) return cs;
#endif
  arr->get(1, &obj1);
  if (!obj1.isStream()) {
    error(-1, "Bad ICCBased color space (stream)");
    obj1.free();
    return NULL;
  }
  dict = obj1.streamGetDict();
  if (!dict->lookup("N", &obj2)->isInt()) {
    error(-1, "Bad ICCBased color space (N)");
    obj2.free();
    obj1.free();
    return NULL;
  }
  nCompsA = obj2.getInt();
  obj2.free();
  if (nCompsA > gfxColorMaxComps) {
    error(-1, "ICCBased color space with too many (%d > %d) components",
	  nCompsA, gfxColorMaxComps);
    nCompsA = gfxColorMaxComps;
  }
  if (dict->lookup("Alternate", &obj2)->isNull() ||
      !(altA = GfxColorSpace::parse(&obj2))) {
    switch (nCompsA) {
    case 1:
      altA = new GfxDeviceGrayColorSpace();
      break;
    case 3:
      altA = new GfxDeviceRGBColorSpace();
      break;
    case 4:
      altA = new GfxDeviceCMYKColorSpace();
      break;
    default:
      error(-1, "Bad ICCBased color space - invalid N");
      obj2.free();
      obj1.free();
      return NULL;
    }
  }
  obj2.free();
  cs = new GfxICCBasedColorSpace(nCompsA, altA, &iccProfileStreamA);
  if (dict->lookup("Range", &obj2)->isArray() &&
      obj2.arrayGetLength() == 2 * nCompsA) {
    for (i = 0; i < nCompsA; ++i) {
      obj2.arrayGet(2*i, &obj3);
      cs->rangeMin[i] = obj3.getNum();
      obj3.free();
      obj2.arrayGet(2*i+1, &obj3);
      cs->rangeMax[i] = obj3.getNum();
      obj3.free();
    }
  }
  obj2.free();
  obj1.free();

#ifdef USE_CMS
  arr->get(1, &obj1);
  dict = obj1.streamGetDict();
  Guchar *profBuf;
  unsigned int bufSize;
  Stream *iccStream = obj1.getStream();
  int c;
  unsigned int size = 0;

  bufSize = 65536;
  profBuf = (Guchar *)gmallocn(bufSize,1);
  iccStream->reset();
  while ((c = iccStream->getChar()) != EOF) {
    if (bufSize <= size) {
      bufSize += 65536;
      profBuf = (Guchar *)greallocn(profBuf,bufSize,1);
    }
    profBuf[size++] = c;
  }
  cmsHPROFILE hp = cmsOpenProfileFromMem(profBuf,size);
  gfree(profBuf);
  if (hp == 0) {
    error(-1, "read ICCBased color space profile error");
  } else {
    cmsHPROFILE dhp = displayProfile;
    if (dhp == NULL) dhp = RGBProfile;
    unsigned int cst = getCMSColorSpaceType(cmsGetColorSpace(hp));
    unsigned int dNChannels = getCMSNChannels(cmsGetColorSpace(dhp));
    unsigned int dcst = getCMSColorSpaceType(cmsGetColorSpace(dhp));
    cmsHTRANSFORM transform;
    if ((transform = cmsCreateTransform(hp,
	   COLORSPACE_SH(cst) |CHANNELS_SH(nCompsA) | BYTES_SH(1),
	   dhp,
	   COLORSPACE_SH(dcst) |
	     CHANNELS_SH(dNChannels) | BYTES_SH(1),
	  INTENT_RELATIVE_COLORIMETRIC,0)) == 0) {
      error(-1, "Can't create transform");
    }
    cs->transform = new GfxColorTransform(transform);
    if (dcst == PT_RGB) {
      if ((transform = cmsCreateTransform(hp,
	    CHANNELS_SH(nCompsA) | BYTES_SH(1),dhp,
	    TYPE_RGB_8,INTENT_RELATIVE_COLORIMETRIC,0)) == 0) {
	error(-1, "Can't create transform");
      }
      cs->lineTransform = new GfxColorTransform(transform);
    }
    cmsCloseProfile(hp);
  }
  obj1.free();
  if (iccProfileStreamA.num > 0) {
    GfxICCBasedCache::put(iccProfileStreamA.num,iccProfileStreamA.gen,cs);
  }
#endif
  return cs;
}
