GfxState::GfxState(GfxState *state) {
  int i;

  memcpy(this, state, sizeof(GfxState));
  if (fillColorSpace) {
    fillColorSpace = state->fillColorSpace->copy();
  }
  if (strokeColorSpace) {
    strokeColorSpace = state->strokeColorSpace->copy();
  }
  if (fillPattern) {
    fillPattern = state->fillPattern->copy();
  }
  if (strokePattern) {
    strokePattern = state->strokePattern->copy();
  }
  for (i = 0; i < 4; ++i) {
    if (transfer[i]) {
      transfer[i] = state->transfer[i]->copy();
    }
  }
  if (lineDashLength > 0) {
    lineDash = (double *)gmallocn(lineDashLength, sizeof(double));
    memcpy(lineDash, state->lineDash, lineDashLength * sizeof(double));
  }
  if (font)
    font->incRefCnt();

  saved = NULL;
}
