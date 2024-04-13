GfxImageColorMap::GfxImageColorMap(GfxImageColorMap *colorMap) {
  int n, i, k;

  colorSpace = colorMap->colorSpace->copy();
  bits = colorMap->bits;
  nComps = colorMap->nComps;
  nComps2 = colorMap->nComps2;
  colorSpace2 = NULL;
  for (k = 0; k < gfxColorMaxComps; ++k) {
    lookup[k] = NULL;
  }
  n = 1 << bits;
  if (colorSpace->getMode() == csIndexed) {
    colorSpace2 = ((GfxIndexedColorSpace *)colorSpace)->getBase();
    for (k = 0; k < nComps2; ++k) {
      lookup[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
      memcpy(lookup[k], colorMap->lookup[k], n * sizeof(GfxColorComp));
    }
  } else if (colorSpace->getMode() == csSeparation) {
    colorSpace2 = ((GfxSeparationColorSpace *)colorSpace)->getAlt();
    for (k = 0; k < nComps2; ++k) {
      lookup[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
      memcpy(lookup[k], colorMap->lookup[k], n * sizeof(GfxColorComp));
    }
  } else {
    for (k = 0; k < nComps; ++k) {
      lookup[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
      memcpy(lookup[k], colorMap->lookup[k], n * sizeof(GfxColorComp));
    }
  }
  for (i = 0; i < nComps; ++i) {
    decodeLow[i] = colorMap->decodeLow[i];
    decodeRange[i] = colorMap->decodeRange[i];
  }
  ok = gTrue;
}
