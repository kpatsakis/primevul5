static BOOL optimize_class_chars(compiler_common *common, const sljit_u8 *bits, BOOL nclass, BOOL invert, jump_list **backtracks)
{
/* May destroy TMP1. */
DEFINE_COMPILER;
uint16_t char_list[MAX_CLASS_CHARS_SIZE];
uint8_t byte;
sljit_s32 type;
int i, j, k, len, c;

if (!sljit_has_cpu_feature(SLJIT_HAS_CMOV))
  return FALSE;

len = 0;

for (i = 0; i < 32; i++)
  {
  byte = bits[i];

  if (nclass)
    byte = ~byte;

  j = 0;
  while (byte != 0)
    {
    if (byte & 0x1)
      {
      c = i * 8 + j;

      k = len;

      if ((c & 0x20) != 0)
        {
        for (k = 0; k < len; k++)
          if (char_list[k] == c - 0x20)
            {
            char_list[k] |= 0x120;
            break;
            }
        }

      if (k == len)
        {
        if (len >= MAX_CLASS_CHARS_SIZE)
          return FALSE;

        char_list[len++] = (uint16_t) c;
        }
      }

    byte >>= 1;
    j++;
    }
  }

if (len == 0) return FALSE;  /* Should never occur, but stops analyzers complaining. */

i = 0;
j = 0;

if (char_list[0] == 0)
  {
  i++;
  OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, 0);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_ZERO);
  }
else
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 0);

while (i < len)
  {
  if ((char_list[i] & 0x100) != 0)
    j++;
  else
    {
    OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, char_list[i]);
    CMOV(SLJIT_ZERO, TMP2, TMP1, 0);
    }
  i++;
  }

if (j != 0)
  {
  OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x20);

  for (i = 0; i < len; i++)
    if ((char_list[i] & 0x100) != 0)
      {
      j--;
      OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, char_list[i] & 0xff);
      CMOV(SLJIT_ZERO, TMP2, TMP1, 0);
      }
  }

if (invert)
  nclass = !nclass;

type = nclass ? SLJIT_NOT_EQUAL : SLJIT_EQUAL;
add_jump(compiler, backtracks, CMP(type, TMP2, 0, SLJIT_IMM, 0));
return TRUE;
}