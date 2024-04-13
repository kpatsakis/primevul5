GBool Matrix::invertTo(Matrix *other)
{
  double det;

  det = 1 / (m[0] * m[3] - m[1] * m[2]);
  other->m[0] = m[3] * det;
  other->m[1] = -m[1] * det;
  other->m[2] = -m[2] * det;
  other->m[3] = m[0] * det;
  other->m[4] = (m[2] * m[5] - m[3] * m[4]) * det;
  other->m[5] = (m[1] * m[4] - m[0] * m[5]) * det;

  return gTrue;
}
