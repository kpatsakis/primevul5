static void cvtHSVToRGB(int h, int s, int v, Guchar *r, Guchar *g, Guchar *b) {
  int x, f, cmax, cmid, cmin;

  if (s == 0) {
    *r = *g = *b = v;
  } else {
    x = h / 60;
    f = h % 60;
    cmax = v;
    if (x & 1) {
      cmid = div255(v * 255 - ((s * f) / 60));
    } else {
      cmid = div255(v * (255 - ((s * (60 - f)) / 60)));
    }
    cmin = div255(v * (255 - s));
    switch (x) {
    case 0: *r = cmax; *g = cmid; *b = cmin; break;
    case 1: *g = cmax; *r = cmid; *b = cmin; break;
    case 2: *g = cmax; *b = cmid; *r = cmin; break;
    case 3: *b = cmax; *g = cmid; *r = cmin; break;
    case 4: *b = cmax; *r = cmid; *g = cmin; break;
    case 5: *r = cmax; *b = cmid; *g = cmin; break;
    }
  }
}
