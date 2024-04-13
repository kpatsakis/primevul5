static SLJIT_INLINE void add_prefix_char(PCRE2_UCHAR chr, fast_forward_char_data *chars, BOOL last)
{
sljit_u32 i, count = chars->count;

if (count == 255)
  return;

if (count == 0)
  {
  chars->count = 1;
  chars->chars[0] = chr;

  if (last)
    chars->last_count = 1;
  return;
  }

for (i = 0; i < count; i++)
  if (chars->chars[i] == chr)
    return;

if (count >= MAX_DIFF_CHARS)
  {
  chars->count = 255;
  return;
  }

chars->chars[count] = chr;
chars->count = count + 1;

if (last)
  chars->last_count++;
}