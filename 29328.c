SplashError SplashBitmap::writePNMFile(char *fileName) {
  FILE *f;
  SplashError e;

  if (!(f = fopen(fileName, "wb"))) {
    return splashErrOpenFile;
  }

  e = this->writePNMFile(f);
  
  fclose(f);
  return e;
}
