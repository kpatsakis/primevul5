void Splash::clearModRegion() {
  modXMin = bitmap->getWidth();
  modYMin = bitmap->getHeight();
  modXMax = -1;
  modYMax = -1;
}
