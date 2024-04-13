Splash::~Splash() {
  while (state->next) {
    restoreState();
  }
  delete state;
  if (vectorAntialias) {
    delete aaBuf;
  }
}
