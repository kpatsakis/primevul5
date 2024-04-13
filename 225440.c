static inline void CoefficientsToAffineArgs(double *coeff)
{
  /* map  internal c0,c1,c2,c3,c4,c5  to  external sx,ry,rx,sy,tx,ty */
  double tmp[4];  /* note indexes 0 and 5 remain unchanged */
  tmp[0]=coeff[3]; tmp[1]=coeff[1]; tmp[2]=coeff[4]; tmp[3]=coeff[2];
  coeff[1]=tmp[0]; coeff[2]=tmp[1]; coeff[3]=tmp[2]; coeff[4]=tmp[3];
}