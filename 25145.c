bool Item_func_like::turboBM_matches(const char* text, int text_len) const
{
  int bcShift;
  int turboShift;
  int shift = pattern_len;
  int j     = 0;
  int u     = 0;
  CHARSET_INFO	*cs= cmp_collation.collation;

  const int plm1=  pattern_len - 1;
  const int tlmpl= text_len - pattern_len;

  /* Searching */
  if (!cs->sort_order)
  {
    while (j <= tlmpl)
    {
      int i= plm1;
      while (i >= 0 && pattern[i] == text[i + j])
      {
	i--;
	if (i == plm1 - shift)
	  i-= u;
      }
      if (i < 0)
	return 1;

      const int v= plm1 - i;
      turboShift = u - v;
      bcShift    = bmBc[(uint) (uchar) text[i + j]] - plm1 + i;
      shift      = MY_MAX(turboShift, bcShift);
      shift      = MY_MAX(shift, bmGs[i]);
      if (shift == bmGs[i])
	u = MY_MIN(pattern_len - shift, v);
      else
      {
	if (turboShift < bcShift)
	  shift = MY_MAX(shift, u + 1);
	u = 0;
      }
      j+= shift;
    }
    return 0;
  }
  else
  {
    while (j <= tlmpl)
    {
      int i= plm1;
      while (i >= 0 && likeconv(cs,pattern[i]) == likeconv(cs,text[i + j]))
      {
	i--;
	if (i == plm1 - shift)
	  i-= u;
      }
      if (i < 0)
	return 1;

      const int v= plm1 - i;
      turboShift = u - v;
      bcShift    = bmBc[(uint) likeconv(cs, text[i + j])] - plm1 + i;
      shift      = MY_MAX(turboShift, bcShift);
      shift      = MY_MAX(shift, bmGs[i]);
      if (shift == bmGs[i])
	u = MY_MIN(pattern_len - shift, v);
      else
      {
	if (turboShift < bcShift)
	  shift = MY_MAX(shift, u + 1);
	u = 0;
      }
      j+= shift;
    }
    return 0;
  }
}