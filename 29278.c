void Splash::fillGlyph2(int x0, int y0, SplashGlyphBitmap *glyph, GBool noClip) {
  SplashPipe pipe;
  int alpha0, alpha;
  Guchar *p;
  int x1, y1, xx, xx1, yy;

  p = glyph->data;
  int xStart = x0 - glyph->x;
  int yStart = y0 - glyph->y;
  int xxLimit = glyph->w;
  int yyLimit = glyph->h;

  if (yStart < 0)
  {
    p += glyph->w * -yStart; // move p to the beginning of the first painted row
    yyLimit += yStart;
    yStart = 0;
  }

  if (xStart < 0)
  {
    p += -xStart; // move p to the first painted pixel
    xxLimit += xStart;
    xStart = 0;
  }

  if (xxLimit + xStart >= bitmap->width) xxLimit = bitmap->width - xStart;
  if (yyLimit + yStart >= bitmap->height) yyLimit = bitmap->height - yStart;

  if (noClip) {
    if (glyph->aa) {
      pipeInit(&pipe, xStart, yStart,
               state->fillPattern, NULL, state->fillAlpha, gTrue, gFalse);
      for (yy = 0, y1 = yStart; yy < yyLimit; ++yy, ++y1) {
        pipeSetXY(&pipe, xStart, y1);
        for (xx = 0, x1 = xStart; xx < xxLimit; ++xx, ++x1) {
          alpha = p[xx];
          if (alpha != 0) {
            pipe.shape = (SplashCoord)(alpha / 255.0);
            pipeRun(&pipe);
            updateModX(x1);
            updateModY(y1);
          } else {
            pipeIncX(&pipe);
          }
        }
        p += glyph->w;
      }
    } else {
      const int widthEight = splashCeil(glyph->w / 8.0);

      pipeInit(&pipe, xStart, yStart,
               state->fillPattern, NULL, state->fillAlpha, gFalse, gFalse);
      for (yy = 0, y1 = yStart; yy < yyLimit; ++yy, ++y1) {
        pipeSetXY(&pipe, xStart, y1);
        for (xx = 0, x1 = xStart; xx < xxLimit; xx += 8) {
          alpha0 = p[xx / 8];
          for (xx1 = 0; xx1 < 8 && xx + xx1 < xxLimit; ++xx1, ++x1) {
            if (alpha0 & 0x80) {
              pipeRun(&pipe);
              updateModX(x1);
              updateModY(y1);
            } else {
              pipeIncX(&pipe);
            }
            alpha0 <<= 1;
          }
        }
        p += widthEight;
      }
    }
  } else {
    if (glyph->aa) {
      pipeInit(&pipe, xStart, yStart,
               state->fillPattern, NULL, state->fillAlpha, gTrue, gFalse);
      for (yy = 0, y1 = yStart; yy < yyLimit; ++yy, ++y1) {
        pipeSetXY(&pipe, xStart, y1);
        for (xx = 0, x1 = xStart; xx < xxLimit; ++xx, ++x1) {
          if (state->clip->test(x1, y1)) {
            alpha = p[xx];
            if (alpha != 0) {
              pipe.shape = (SplashCoord)(alpha / 255.0);
              pipeRun(&pipe);
              updateModX(x1);
              updateModY(y1);
            } else {
              pipeIncX(&pipe);
            }
          } else {
            pipeIncX(&pipe);
          }
        }
        p += glyph->w;
      }
    } else {
      const int widthEight = splashCeil(glyph->w / 8.0);

      pipeInit(&pipe, xStart, yStart,
               state->fillPattern, NULL, state->fillAlpha, gFalse, gFalse);
      for (yy = 0, y1 = yStart; yy < yyLimit; ++yy, ++y1) {
        pipeSetXY(&pipe, xStart, y1);
        for (xx = 0, x1 = xStart; xx < xxLimit; xx += 8) {
          alpha0 = p[xx / 8];
          for (xx1 = 0; xx1 < 8 && xx + xx1 < xxLimit; ++xx1, ++x1) {
            if (state->clip->test(x1, y1)) {
              if (alpha0 & 0x80) {
                pipeRun(&pipe);
                updateModX(x1);
                updateModY(y1);
              } else {
                pipeIncX(&pipe);
              }
            } else {
              pipeIncX(&pipe);
            }
            alpha0 <<= 1;
          }
        }
        p += widthEight;
      }
    }
  }
}
