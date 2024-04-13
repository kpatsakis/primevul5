static void InvertPerspectiveCoefficients(const double *coeff,
  double *inverse)
{
  /* From "Digital Image Warping" by George Wolberg, page 53 */
  double determinant;

  determinant=PerceptibleReciprocal(coeff[0]*coeff[4]-coeff[3]*coeff[1]);
  inverse[0]=determinant*(coeff[4]-coeff[7]*coeff[5]);
  inverse[1]=determinant*(coeff[7]*coeff[2]-coeff[1]);
  inverse[2]=determinant*(coeff[1]*coeff[5]-coeff[4]*coeff[2]);
  inverse[3]=determinant*(coeff[6]*coeff[5]-coeff[3]);
  inverse[4]=determinant*(coeff[0]-coeff[6]*coeff[2]);
  inverse[5]=determinant*(coeff[3]*coeff[2]-coeff[0]*coeff[5]);
  inverse[6]=determinant*(coeff[3]*coeff[7]-coeff[6]*coeff[4]);
  inverse[7]=determinant*(coeff[6]*coeff[1]-coeff[0]*coeff[7]);
}