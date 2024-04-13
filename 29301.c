inline void Splash::pipeInit(SplashPipe *pipe, int x, int y,
			     SplashPattern *pattern, SplashColorPtr cSrc,
			     SplashCoord aInput, GBool usesShape,
			     GBool nonIsolatedGroup) {
  pipeSetXY(pipe, x, y);
  pipe->pattern = NULL;

  if (pattern) {
    if (pattern->isStatic()) {
      pattern->getColor(x, y, pipe->cSrcVal);
    } else {
      pipe->pattern = pattern;
    }
    pipe->cSrc = pipe->cSrcVal;
  } else {
    pipe->cSrc = cSrc;
  }

  pipe->aInput = aInput;
  if (!state->softMask) {
    if (usesShape) {
      pipe->aInput *= 255;
    } else {
      pipe->aSrc = (Guchar)splashRound(pipe->aInput * 255);
    }
  }
  pipe->usesShape = usesShape;

  if (aInput == 1 && !state->softMask && !usesShape &&
      !state->inNonIsolatedGroup) {
    pipe->noTransparency = gTrue;
  } else {
    pipe->noTransparency = gFalse;
  }

  if (pipe->noTransparency) {
    pipe->resultColorCtrl = pipeResultColorNoAlphaBlend[bitmap->mode];
  } else if (!state->blendFunc) {
    pipe->resultColorCtrl = pipeResultColorAlphaNoBlend[bitmap->mode];
  } else {
    pipe->resultColorCtrl = pipeResultColorAlphaBlend[bitmap->mode];
  }

  if (nonIsolatedGroup) {
    pipe->nonIsolatedGroup = splashColorModeNComps[bitmap->mode];
  } else {
    pipe->nonIsolatedGroup = 0;
  }
}
