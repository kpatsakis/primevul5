void GfxCalRGBColorSpace::getXYZ(GfxColor *color, 
  double *pX, double *pY, double *pZ) {
  double A, B, C;

  A = colToDbl(color->c[0]);
  B = colToDbl(color->c[1]);
  C = colToDbl(color->c[2]);
  *pX = mat[0]*pow(A,gammaR)+mat[3]*pow(B,gammaG)+mat[6]*pow(C,gammaB);
  *pY = mat[1]*pow(A,gammaR)+mat[4]*pow(B,gammaG)+mat[7]*pow(C,gammaB);
  *pZ = mat[2]*pow(A,gammaR)+mat[5]*pow(B,gammaG)+mat[8]*pow(C,gammaB);
}
