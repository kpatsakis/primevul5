GfxColorSpace *GfxICCBasedColorSpace::copy() {
  GfxICCBasedColorSpace *cs;
  int i;

  cs = new GfxICCBasedColorSpace(nComps, alt->copy(), &iccProfileStream);
  for (i = 0; i < 4; ++i) {
    cs->rangeMin[i] = rangeMin[i];
    cs->rangeMax[i] = rangeMax[i];
  }
#ifdef USE_CMS
  cs->transform = transform;
  if (transform != NULL) transform->ref();
  cs->lineTransform = lineTransform;
  if (lineTransform != NULL) lineTransform->ref();
#endif
  return cs;
}
