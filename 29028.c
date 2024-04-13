GfxPatchMeshShading::GfxPatchMeshShading(GfxPatchMeshShading *shading):
  GfxShading(shading)
{
  int i;

  nPatches = shading->nPatches;
  patches = (GfxPatch *)gmallocn(nPatches, sizeof(GfxPatch));
  memcpy(patches, shading->patches, nPatches * sizeof(GfxPatch));
  nFuncs = shading->nFuncs;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = shading->funcs[i]->copy();
  }
}
