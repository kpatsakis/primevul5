static inline void ConvertAdobe98ToXYZ(const double red,const double green,
  const double blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  /*
    Convert Adobe '98 to XYZ colorspace.
  */
  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*DecodePixelGamma(red);
  g=QuantumScale*DecodePixelGamma(green);
  b=QuantumScale*DecodePixelGamma(blue);
  *X=0.57666904291013050*r+0.18555823790654630*g+0.18822864623499470*b;
  *Y=0.29734497525053605*r+0.62736356625546610*g+0.07529145849399788*b;
  *Z=0.02703136138641234*r+0.07068885253582723*g+0.99133753683763880*b;
}