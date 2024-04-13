GfxFunctionShading::~GfxFunctionShading() {
  int i;

  for (i = 0; i < nFuncs; ++i) {
    delete funcs[i];
  }
}
