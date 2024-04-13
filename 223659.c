static inline void ConvertProPhotoToXYZ(const double red,const double green,
  const double blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  /*
    Convert ProPhoto to XYZ colorspace.
  */
  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*DecodePixelGamma(red);
  g=QuantumScale*DecodePixelGamma(green);
  b=QuantumScale*DecodePixelGamma(blue);
  *X=0.7977604896723027*r+0.13518583717574031*g+0.03134934958152480000*b;
  *Y=0.2880711282292934*r+0.71184321781010140*g+0.00008565396060525902*b;
  *Z=0.0000000000000000*r+0.00000000000000000*g+0.82510460251046010000*b;
}