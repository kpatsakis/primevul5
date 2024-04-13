inline void Splash::updateModY(int y) {
  if (y < modYMin) {
    modYMin = y;
  }
  if (y > modYMax) {
    modYMax = y;
  }
}
