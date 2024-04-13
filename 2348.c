static BOOL check_fast_forward_char_pair_simd(compiler_common *common, fast_forward_char_data *chars, int max)
{
  sljit_s32 i, j, max_i = 0, max_j = 0;
  sljit_u32 max_pri = 0;
  PCRE2_UCHAR a1, a2, a_pri, b1, b2, b_pri;

  for (i = max - 1; i >= 1; i--)
    {
    if (chars[i].last_count > 2)
      {
      a1 = chars[i].chars[0];
      a2 = chars[i].chars[1];
      a_pri = chars[i].last_count;

      j = i - max_fast_forward_char_pair_offset();
      if (j < 0)
        j = 0;

      while (j < i)
        {
        b_pri = chars[j].last_count;
        if (b_pri > 2 && a_pri + b_pri >= max_pri)
          {
          b1 = chars[j].chars[0];
          b2 = chars[j].chars[1];

          if (a1 != b1 && a1 != b2 && a2 != b1 && a2 != b2)
            {
            max_pri = a_pri + b_pri;
            max_i = i;
            max_j = j;
            }
          }
        j++;
        }
      }
    }

if (max_pri == 0)
  return FALSE;

fast_forward_char_pair_simd(common, max_i, chars[max_i].chars[0], chars[max_i].chars[1], max_j, chars[max_j].chars[0], chars[max_j].chars[1]);
return TRUE;
}