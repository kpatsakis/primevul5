static inline void AffineArgsToCoefficients(double *affine)
{
  /* map  external sx,ry,rx,sy,tx,ty  to  internal c0,c2,c4,c1,c3,c5 */
  double tmp[4];  /* note indexes  0 and 5 remain unchanged */
  tmp[0]=affine[1]; tmp[1]=affine[2]; tmp[2]=affine[3]; tmp[3]=affine[4];
  affine[3]=tmp[0]; affine[1]=tmp[1]; affine[4]=tmp[2]; affine[2]=tmp[3];
}