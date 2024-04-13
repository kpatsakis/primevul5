static void InvertAffineCoefficients(const double *coeff,double *inverse)
{
  /* From "Digital Image Warping" by George Wolberg, page 50 */
  double determinant;

  determinant=PerceptibleReciprocal(coeff[0]*coeff[4]-coeff[1]*coeff[3]);
  inverse[0]=determinant*coeff[4];
  inverse[1]=determinant*(-coeff[1]);
  inverse[2]=determinant*(coeff[1]*coeff[5]-coeff[2]*coeff[4]);
  inverse[3]=determinant*(-coeff[3]);
  inverse[4]=determinant*coeff[0];
  inverse[5]=determinant*(coeff[2]*coeff[3]-coeff[0]*coeff[5]);
}