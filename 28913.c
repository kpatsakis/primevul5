static void cvtRGBToHSV(Guchar r, Guchar g, Guchar b, int *h, int *s, int *v) {
  int cmax, cmid, cmin, x;

  if (r >= g) {
    if (g >= b)      { x = 0; cmax = r; cmid = g; cmin = b; }
    else if (b >= r) { x = 4; cmax = b; cmid = r; cmin = g; }
    else             { x = 5; cmax = r; cmid = b; cmin = g; }
  } else {
    if (r >= b)      { x = 1; cmax = g; cmid = r; cmin = b; }
    else if (g >= b) { x = 2; cmax = g; cmid = b; cmin = r; }
    else             { x = 3; cmax = b; cmid = g; cmin = r; }
  }
  if (cmax == cmin) {
    *h = *s = 0;
  } else {
    *h = x * 60;
    if (x & 1) {
      *h += ((cmax - cmid) * 60) / (cmax - cmin);
    } else {
      *h += ((cmid - cmin) * 60) / (cmax - cmin);
    }
    *s = (255 * (cmax - cmin)) / cmax;
  }
  *v = cmax;
}
