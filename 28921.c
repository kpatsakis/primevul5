void SplashOutputDev::endPage() {
  if (colorMode != splashModeMono1) {
    splash->compositeBackground(paperColor);
  }
}
