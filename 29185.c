GfxICCBasedColorSpace::~GfxICCBasedColorSpace() {
  delete alt;
#ifdef USE_CMS
  if (transform != NULL) {
    if (transform->unref() == 0) delete transform;
  }
  if (lineTransform != NULL) {
    if (lineTransform->unref() == 0) delete lineTransform;
  }
#endif
}
