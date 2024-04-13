void Matrix::transform(double x, double y, double *tx, double *ty)
{
  double temp_x, temp_y;

  temp_x = m[0] * x + m[2] * y + m[4];
  temp_y = m[1] * x + m[3] * y + m[5];

  *tx = temp_x;
  *ty = temp_y;
}
