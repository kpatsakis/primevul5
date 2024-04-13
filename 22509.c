static inline void ConvertLabToXYZ(const double L,const double a,const double b,
  double *X,double *Y,double *Z)
{
  double
    x,
    y,
    z;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  y=(L+16.0)/116.0;
  x=y+a/500.0;
  z=y-b/200.0;
  if ((x*x*x) > CIEEpsilon)
    x=(x*x*x);
  else
    x=(116.0*x-16.0)/CIEK;
  if ((y*y*y) > CIEEpsilon)
    y=(y*y*y);
  else
    y=L/CIEK;
  if ((z*z*z) > CIEEpsilon)
    z=(z*z*z);
  else
    z=(116.0*z-16.0)/CIEK;
  *X=D65X*x;
  *Y=D65Y*y;
  *Z=D65Z*z;
}