MagickExport void ConvertRGBToHSB(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *saturation,double *brightness)
{
  double
    b,
    delta,
    g,
    max,
    min,
    r;

  /*
    Convert RGB to HSB colorspace.
  */
  assert(hue != (double *) NULL);
  assert(saturation != (double *) NULL);
  assert(brightness != (double *) NULL);
  *hue=0.0;
  *saturation=0.0;
  *brightness=0.0;
  r=(double) red;
  g=(double) green;
  b=(double) blue;
  min=r < g ? r : g;
  if (b < min)
    min=b;
  max=r > g ? r : g;
  if (b > max)
    max=b;
  if (fabs(max) < MagickEpsilon)
    return;
  delta=max-min;
  *saturation=delta/max;
  *brightness=QuantumScale*max;
  if (fabs(delta) < MagickEpsilon)
    return;
  if (fabs(r-max) < MagickEpsilon)
    *hue=(g-b)/delta;
  else
    if (fabs(g-max) < MagickEpsilon)
      *hue=2.0+(b-r)/delta;
    else
      *hue=4.0+(r-g)/delta;
  *hue/=6.0;
  if (*hue < 0.0)
    *hue+=1.0;
}