static inline void ConvertXYZToDisplayP3(const double X,const double Y,
  const double Z,double *red,double *green,double *blue)
{
  double
    b,
    g,
    r;

  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  r=2.49349691194142500*X-0.93138361791912390*Y-0.402710784450716840*Z;
  g=(-0.82948896956157470)*X+1.76266406031834630*Y+0.023624685841943577*Z;
  b=0.03584583024378447*X-0.07617238926804182*Y+0.956884524007687200*Z;
  *red=EncodePixelGamma(QuantumRange*r);
  *green=EncodePixelGamma(QuantumRange*g);
  *blue=EncodePixelGamma(QuantumRange*b);
}