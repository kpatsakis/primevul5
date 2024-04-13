GfxPatchMeshShading::~GfxPatchMeshShading() {
  int i;

  gfree(patches);
  for (i = 0; i < nFuncs; ++i) {
    delete funcs[i];
  }
}
