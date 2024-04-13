void GfxState::clearPath() {
  delete path;
  path = new GfxPath();
}
