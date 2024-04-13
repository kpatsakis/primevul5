SplashError Splash::stroke(SplashPath *path) {
  SplashPath *path2, *dPath;

  if (debugMode) {
    printf("stroke [dash:%d] [width:%.2f]:\n",
	   state->lineDashLength, (double)state->lineWidth);
    dumpPath(path);
  }
  opClipRes = splashClipAllOutside;
  if (path->length == 0) {
    return splashErrEmptyPath;
  }
  path2 = flattenPath(path, state->matrix, state->flatness);
  if (state->lineDashLength > 0) {
    dPath = makeDashedPath(path2);
    delete path2;
    path2 = dPath;
  }
  if (state->lineWidth == 0) {
    strokeNarrow(path2);
  } else {
    strokeWide(path2);
  }
  delete path2;
  return splashOk;
}
