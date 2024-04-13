static BOOL detect_repeat(compiler_common *common, PCRE2_SPTR begin)
{
PCRE2_SPTR end = bracketend(begin);
PCRE2_SPTR next;
PCRE2_SPTR next_end;
PCRE2_SPTR max_end;
PCRE2_UCHAR type;
sljit_sw length = end - begin;
sljit_s32 min, max, i;

/* Detect fixed iterations first. */
if (end[-(1 + LINK_SIZE)] != OP_KET)
  return FALSE;

/* Already detected repeat. */
if (common->private_data_ptrs[end - common->start - LINK_SIZE] != 0)
  return TRUE;

next = end;
min = 1;
while (1)
  {
  if (*next != *begin)
    break;
  next_end = bracketend(next);
  if (next_end - next != length || memcmp(begin, next, IN_UCHARS(length)) != 0)
    break;
  next = next_end;
  min++;
  }

if (min == 2)
  return FALSE;

max = 0;
max_end = next;
if (*next == OP_BRAZERO || *next == OP_BRAMINZERO)
  {
  type = *next;
  while (1)
    {
    if (next[0] != type || next[1] != OP_BRA || next[2 + LINK_SIZE] != *begin)
      break;
    next_end = bracketend(next + 2 + LINK_SIZE);
    if (next_end - next != (length + 2 + LINK_SIZE) || memcmp(begin, next + 2 + LINK_SIZE, IN_UCHARS(length)) != 0)
      break;
    next = next_end;
    max++;
    }

  if (next[0] == type && next[1] == *begin && max >= 1)
    {
    next_end = bracketend(next + 1);
    if (next_end - next == (length + 1) && memcmp(begin, next + 1, IN_UCHARS(length)) == 0)
      {
      for (i = 0; i < max; i++, next_end += 1 + LINK_SIZE)
        if (*next_end != OP_KET)
          break;

      if (i == max)
        {
        common->private_data_ptrs[max_end - common->start - LINK_SIZE] = next_end - max_end;
        common->private_data_ptrs[max_end - common->start - LINK_SIZE + 1] = (type == OP_BRAZERO) ? OP_UPTO : OP_MINUPTO;
        /* +2 the original and the last. */
        common->private_data_ptrs[max_end - common->start - LINK_SIZE + 2] = max + 2;
        if (min == 1)
          return TRUE;
        min--;
        max_end -= (1 + LINK_SIZE) + GET(max_end, -LINK_SIZE);
        }
      }
    }
  }

if (min >= 3)
  {
  common->private_data_ptrs[end - common->start - LINK_SIZE] = max_end - end;
  common->private_data_ptrs[end - common->start - LINK_SIZE + 1] = OP_EXACT;
  common->private_data_ptrs[end - common->start - LINK_SIZE + 2] = min;
  return TRUE;
  }

return FALSE;
}