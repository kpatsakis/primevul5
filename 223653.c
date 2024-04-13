static inline void ConvertDisplayP3ToXYZ(const double red,const double green,
  const double blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  /*
    Convert Display P3 to XYZ colorspace.
  */
  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*DecodePixelGamma(red);
  g=QuantumScale*DecodePixelGamma(green);
  b=QuantumScale*DecodePixelGamma(blue);
  *X=0.4865709486482162*r+0.26566769316909306*g+0.1982172852343625*b;
  *Y=0.2289745640697488*r+0.69173852183650640*g+0.0792869140937450*b;
  *Z=0.0000000000000000*r+0.04511338185890264*g+1.0439443689009760*b;
}