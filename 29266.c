SplashError Splash::clipToPath(SplashPath *path, GBool eo) {
  return state->clip->clipToPath(path, state->matrix, state->flatness, eo);
}
