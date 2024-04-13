inline void Splash::pipeIncX(SplashPipe *pipe) {
  ++pipe->x;
  if (state->softMask) {
    ++pipe->softMaskPtr;
  }
  switch (bitmap->mode) {
  case splashModeMono1:
    if (!(pipe->destColorMask >>= 1)) {
      pipe->destColorMask = 0x80;
      ++pipe->destColorPtr;
    }
    break;
  case splashModeMono8:
    ++pipe->destColorPtr;
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    pipe->destColorPtr += 3;
    break;
  case splashModeXBGR8:
    pipe->destColorPtr += 4;
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    pipe->destColorPtr += 4;
    break;
#endif
  }
  if (pipe->destAlphaPtr) {
    ++pipe->destAlphaPtr;
  }
  if (pipe->alpha0Ptr) {
    ++pipe->alpha0Ptr;
  }
}
