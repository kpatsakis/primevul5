static inline void ConvertXYZToProPhoto(const double X,const double Y,
  const double Z,double *red,double *green,double *blue)
{
  double
    b,
    g,
    r;

  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  r=1.3457989731028281*X-0.25558010007997534*Y-0.05110628506753401*Z;
  g=(-0.5446224939028347)*X+1.50823274131327810*Y+0.02053603239147973*Z;
  b=0.0000000000000000*X+0.0000000000000000*Y+1.21196754563894540*Z;
  *red=EncodePixelGamma(QuantumRange*r);
  *green=EncodePixelGamma(QuantumRange*g);
  *blue=EncodePixelGamma(QuantumRange*b);
}