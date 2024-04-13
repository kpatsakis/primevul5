GfxPatchMeshShading::GfxPatchMeshShading(int typeA,
					 GfxPatch *patchesA, int nPatchesA,
					 Function **funcsA, int nFuncsA):
  GfxShading(typeA)
{
  int i;

  patches = patchesA;
  nPatches = nPatchesA;
  nFuncs = nFuncsA;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = funcsA[i];
  }
}
