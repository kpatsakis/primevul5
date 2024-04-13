static inline void ConvertXYZToLuv(const double X,const double Y,const double Z,
  double *L,double *u,double *v)
{
  double
    alpha;

  assert(L != (double *) NULL);
  assert(u != (double *) NULL);
  assert(v != (double *) NULL);
  if ((Y/D65Y) > CIEEpsilon)
    *L=(double) (116.0*pow(Y/D65Y,1.0/3.0)-16.0);
  else
    *L=CIEK*(Y/D65Y);
  alpha=PerceptibleReciprocal(X+15.0*Y+3.0*Z);
  *u=13.0*(*L)*((4.0*alpha*X)-(4.0*D65X/(D65X+15.0*D65Y+3.0*D65Z)));
  *v=13.0*(*L)*((9.0*alpha*Y)-(9.0*D65Y/(D65X+15.0*D65Y+3.0*D65Z)));
  *L/=100.0;
  *u=(*u+134.0)/354.0;
  *v=(*v+140.0)/262.0;
}