void Splash::setMatrix(SplashCoord *matrix) {
  memcpy(state->matrix, matrix, 6 * sizeof(SplashCoord));
}
