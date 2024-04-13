MagickExport void ConvertRGBToHWB(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *whiteness,double *blackness)
{
  double
    b,
    f,
    g,
    p,
    r,
    v,
    w;

  /*
    Convert RGB to HWB colorspace.
  */
  assert(hue != (double *) NULL);
  assert(whiteness != (double *) NULL);
  assert(blackness != (double *) NULL);
  r=(double) red;
  g=(double) green;
  b=(double) blue;
  w=MagickMin(r,MagickMin(g,b));
  v=MagickMax(r,MagickMax(g,b));
  *blackness=1.0-QuantumScale*v;
  *whiteness=QuantumScale*w;
  if (fabs(v-w) < MagickEpsilon)
    {
      *hue=(-1.0);
      return;
    }
  f=(fabs(r-w) < MagickEpsilon) ? g-b : ((fabs(g-w) < MagickEpsilon) ? b-r : r-g);
  p=(fabs(r-w) < MagickEpsilon) ? 3.0 : ((fabs(g-w) < MagickEpsilon) ? 5.0 : 1.0);
  *hue=(p-f/(v-1.0*w))/6.0;
}