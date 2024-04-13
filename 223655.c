static inline void ConvertXYZToAdobe98(const double X,const double Y,
  const double Z,double *red,double *green,double *blue)
{
  double
    b,
    g,
    r;

  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  r=2.041587903810746500*X-0.56500697427885960*Y-0.34473135077832956*Z;
  g=(-0.969243636280879500)*X+1.87596750150772020*Y+0.04155505740717557*Z;
  b=0.013444280632031142*X-0.11836239223101838*Y+1.01517499439120540*Z;
  *red=EncodePixelGamma(QuantumRange*r);
  *green=EncodePixelGamma(QuantumRange*g);
  *blue=EncodePixelGamma(QuantumRange*b);
}