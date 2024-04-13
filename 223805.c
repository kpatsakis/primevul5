MagickExport void ConvertRGBToHCL(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *chroma,double *luma)
{
  double
    b,
    c,
    g,
    h,
    max,
    r;

  /*
    Convert RGB to HCL colorspace.
  */
  assert(hue != (double *) NULL);
  assert(chroma != (double *) NULL);
  assert(luma != (double *) NULL);
  r=(double) red;
  g=(double) green;
  b=(double) blue;
  max=MagickMax(r,MagickMax(g,b));
  c=max-(double) MagickMin(r,MagickMin(g,b));
  h=0.0;
  if (fabs(c) < MagickEpsilon)
    h=0.0;
  else
    if (red == (Quantum) max)
      h=fmod((g-b)/c+6.0,6.0);
    else
      if (green == (Quantum) max)
        h=((b-r)/c)+2.0;
      else
        if (blue == (Quantum) max)
          h=((r-g)/c)+4.0;
  *hue=(h/6.0);
  *chroma=QuantumScale*c;
  *luma=QuantumScale*(0.298839*r+0.586811*g+0.114350*b);
}