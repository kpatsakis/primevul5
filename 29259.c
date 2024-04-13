void PSOutputDev::clipToStrokePath(GfxState *state) {
  doPath(state->getPath());
  writePS("Ws\n");
}
