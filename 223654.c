static inline void ConvertXYZToRGB(const double X,const double Y,const double Z,
  double *red,double *green,double *blue)
{
  double
    b,
    g,
    r;

  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  r=3.2404542*X-1.5371385*Y-0.4985314*Z;
  g=(-0.9692660)*X+1.8760108*Y+0.0415560*Z;
  b=0.0556434*X-0.2040259*Y+1.0572252*Z;
  *red=EncodePixelGamma(QuantumRange*r);
  *green=EncodePixelGamma(QuantumRange*g);
  *blue=EncodePixelGamma(QuantumRange*b);
}