void GfxState::setTransfer(Function **funcs) {
  int i;

  for (i = 0; i < 4; ++i) {
    if (transfer[i]) {
      delete transfer[i];
    }
    transfer[i] = funcs[i];
  }
}
