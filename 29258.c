void PSOutputDev::clip(GfxState *state) {
  doPath(state->getPath());
  writePS("W\n");
}
