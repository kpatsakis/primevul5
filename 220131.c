void LibRaw::ljpeg_idct(struct jhead *jh)
{
  int c, i, j, len, skip, coef;
  float work[3][8][8];
  static float cs[106] = {0};
  static const uchar zigzag[80] = {
      0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,
      12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28,
      35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
      58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

  if (!cs[0])
    FORC(106) cs[c] = cos((c & 31) * M_PI / 16) / 2;
  memset(work, 0, sizeof work);
  work[0][0][0] = jh->vpred[0] += ljpeg_diff(jh->huff[0]) * jh->quant[0];
  for (i = 1; i < 64; i++)
  {
    len = gethuff(jh->huff[16]);
    i += skip = len >> 4;
    if (!(len &= 15) && skip < 15)
      break;
    coef = getbits(len);
    if ((coef & (1 << (len - 1))) == 0)
      coef -= (1 << len) - 1;
    ((float *)work)[zigzag[i]] = coef * jh->quant[i];
  }
  FORC(8) work[0][0][c] *= M_SQRT1_2;
  FORC(8) work[0][c][0] *= M_SQRT1_2;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      FORC(8) work[1][i][j] += work[0][i][c] * cs[(j * 2 + 1) * c];
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      FORC(8) work[2][i][j] += work[1][c][j] * cs[(i * 2 + 1) * c];

  FORC(64) jh->idct[c] = CLIP(((float *)work[2])[c] + 0.5);
}