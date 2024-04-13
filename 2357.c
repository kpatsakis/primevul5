static BOOL optimize_class_ranges(compiler_common *common, const sljit_u8 *bits, BOOL nclass, BOOL invert, jump_list **backtracks)
{
/* May destroy TMP1. */
DEFINE_COMPILER;
int ranges[MAX_CLASS_RANGE_SIZE];
sljit_u8 bit, cbit, all;
int i, byte, length = 0;

bit = bits[0] & 0x1;
/* All bits will be zero or one (since bit is zero or one). */
all = -bit;

for (i = 0; i < 256; )
  {
  byte = i >> 3;
  if ((i & 0x7) == 0 && bits[byte] == all)
    i += 8;
  else
    {
    cbit = (bits[byte] >> (i & 0x7)) & 0x1;
    if (cbit != bit)
      {
      if (length >= MAX_CLASS_RANGE_SIZE)
        return FALSE;
      ranges[length] = i;
      length++;
      bit = cbit;
      all = -cbit;
      }
    i++;
    }
  }

if (((bit == 0) && nclass) || ((bit == 1) && !nclass))
  {
  if (length >= MAX_CLASS_RANGE_SIZE)
    return FALSE;
  ranges[length] = 256;
  length++;
  }

if (length < 0 || length > 4)
  return FALSE;

bit = bits[0] & 0x1;
if (invert) bit ^= 0x1;

/* No character is accepted. */
if (length == 0 && bit == 0)
  add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));

switch(length)
  {
  case 0:
  /* When bit != 0, all characters are accepted. */
  return TRUE;

  case 1:
  add_jump(compiler, backtracks, CMP(bit == 0 ? SLJIT_LESS : SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, ranges[0]));
  return TRUE;

  case 2:
  if (ranges[0] + 1 != ranges[1])
    {
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[0]);
    add_jump(compiler, backtracks, CMP(bit != 0 ? SLJIT_LESS : SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, ranges[1] - ranges[0]));
    }
  else
    add_jump(compiler, backtracks, CMP(bit != 0 ? SLJIT_EQUAL : SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[0]));
  return TRUE;

  case 3:
  if (bit != 0)
    {
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, ranges[2]));
    if (ranges[0] + 1 != ranges[1])
      {
      OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[0]);
      add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[1] - ranges[0]));
      }
    else
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[0]));
    return TRUE;
    }

  add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[0]));
  if (ranges[1] + 1 != ranges[2])
    {
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[1]);
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[2] - ranges[1]));
    }
  else
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[1]));
  return TRUE;

  case 4:
  if ((ranges[1] - ranges[0]) == (ranges[3] - ranges[2])
      && (ranges[0] | (ranges[2] - ranges[0])) == ranges[2]
      && (ranges[1] & (ranges[2] - ranges[0])) == 0
      && is_powerof2(ranges[2] - ranges[0]))
    {
    SLJIT_ASSERT((ranges[0] & (ranges[2] - ranges[0])) == 0 && (ranges[2] & ranges[3] & (ranges[2] - ranges[0])) != 0);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[2] - ranges[0]);
    if (ranges[2] + 1 != ranges[3])
      {
      OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[2]);
      add_jump(compiler, backtracks, CMP(bit != 0 ? SLJIT_LESS : SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, ranges[3] - ranges[2]));
      }
    else
      add_jump(compiler, backtracks, CMP(bit != 0 ? SLJIT_EQUAL : SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[2]));
    return TRUE;
    }

  if (bit != 0)
    {
    i = 0;
    if (ranges[0] + 1 != ranges[1])
      {
      OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[0]);
      add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[1] - ranges[0]));
      i = ranges[0];
      }
    else
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[0]));

    if (ranges[2] + 1 != ranges[3])
      {
      OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[2] - i);
      add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[3] - ranges[2]));
      }
    else
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[2] - i));
    return TRUE;
    }

  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[0]);
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, ranges[3] - ranges[0]));
  if (ranges[1] + 1 != ranges[2])
    {
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ranges[1] - ranges[0]);
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, ranges[2] - ranges[1]));
    }
  else
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, ranges[1] - ranges[0]));
  return TRUE;

  default:
  SLJIT_UNREACHABLE();
  return FALSE;
  }
}