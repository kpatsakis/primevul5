static BOOL check_fast_forward_char_pair_sse2(compiler_common *common, fast_forward_char_data *chars, int max)
{
sljit_s32 i, j, priority, count;
sljit_u32 priorities;
PCRE2_UCHAR a1, a2, b1, b2;

priorities = 0;

count = 0;
for (i = 0; i < max; i++)
  {
  if (chars[i].last_count > 2)
    {
    SLJIT_ASSERT(chars[i].last_count <= 7);

    priorities |= (1 << chars[i].last_count);
    count++;
    }
  }

if (count < 2)
  return FALSE;

for (priority = 7; priority > 2; priority--)
  {
  if ((priorities & (1 << priority)) == 0)
    continue;

  for (i = max - 1; i >= 1; i--)
    if (chars[i].last_count >= priority)
      {
      SLJIT_ASSERT(chars[i].count <= 2 && chars[i].count >= 1);

      a1 = chars[i].chars[0];
      a2 = chars[i].chars[1];

      j = i - max_fast_forward_char_pair_sse2_offset();
      if (j < 0)
        j = 0;

      while (j < i)
        {
        if (chars[j].last_count >= priority)
          {
          b1 = chars[j].chars[0];
          b2 = chars[j].chars[1];

          if (a1 != b1 && a1 != b2 && a2 != b1 && a2 != b2)
            {
            fast_forward_char_pair_sse2(common, i, a1, a2, j, b1, b2);
            return TRUE;
            }
          }
        j++;
        }
      }
  }

return FALSE;
}