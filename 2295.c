static SLJIT_INLINE BOOL fast_forward_first_n_chars(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_label *start;
struct sljit_jump *match;
fast_forward_char_data chars[MAX_N_CHARS];
sljit_s32 offset;
PCRE2_UCHAR mask;
PCRE2_UCHAR *char_set, *char_set_end;
int i, max, from;
int range_right = -1, range_len;
sljit_u8 *update_table = NULL;
BOOL in_range;
sljit_u32 rec_count;

for (i = 0; i < MAX_N_CHARS; i++)
  {
  chars[i].count = 0;
  chars[i].last_count = 0;
  }

rec_count = 10000;
max = scan_prefix(common, common->start, chars, MAX_N_CHARS, &rec_count);

if (max < 1)
  return FALSE;

/* Convert last_count to priority. */
for (i = 0; i < max; i++)
  {
  SLJIT_ASSERT(chars[i].count > 0 && chars[i].last_count <= chars[i].count);

  if (chars[i].count == 1)
    {
    chars[i].last_count = (chars[i].last_count == 1) ? 7 : 5;
    /* Simplifies algorithms later. */
    chars[i].chars[1] = chars[i].chars[0];
    }
  else if (chars[i].count == 2)
    {
    SLJIT_ASSERT(chars[i].chars[0] != chars[i].chars[1]);

    if (is_powerof2(chars[i].chars[0] ^ chars[i].chars[1]))
      chars[i].last_count = (chars[i].last_count == 2) ? 6 : 4;
    else
      chars[i].last_count = (chars[i].last_count == 2) ? 3 : 2;
    }
  else
    chars[i].last_count = (chars[i].count == 255) ? 0 : 1;
  }

#ifdef JIT_HAS_FAST_FORWARD_CHAR_PAIR_SIMD
if (JIT_HAS_FAST_FORWARD_CHAR_PAIR_SIMD && check_fast_forward_char_pair_simd(common, chars, max))
  return TRUE;
#endif

in_range = FALSE;
/* Prevent compiler "uninitialized" warning */
from = 0;
range_len = 4 /* minimum length */ - 1;
for (i = 0; i <= max; i++)
  {
  if (in_range && (i - from) > range_len && (chars[i - 1].count < 255))
    {
    range_len = i - from;
    range_right = i - 1;
    }

  if (i < max && chars[i].count < 255)
    {
    SLJIT_ASSERT(chars[i].count > 0);
    if (!in_range)
      {
      in_range = TRUE;
      from = i;
      }
    }
  else
    in_range = FALSE;
  }

if (range_right >= 0)
  {
  update_table = (sljit_u8 *)allocate_read_only_data(common, 256);
  if (update_table == NULL)
    return TRUE;
  memset(update_table, IN_UCHARS(range_len), 256);

  for (i = 0; i < range_len; i++)
    {
    SLJIT_ASSERT(chars[range_right - i].count > 0 && chars[range_right - i].count < 255);

    char_set = chars[range_right - i].chars;
    char_set_end = char_set + chars[range_right - i].count;
    do
      {
      if (update_table[(*char_set) & 0xff] > IN_UCHARS(i))
        update_table[(*char_set) & 0xff] = IN_UCHARS(i);
      char_set++;
      }
    while (char_set < char_set_end);
    }
  }

offset = -1;
/* Scan forward. */
for (i = 0; i < max; i++)
  {
  if (range_right == i)
    continue;

  if (offset == -1)
    {
    if (chars[i].last_count >= 2)
      offset = i;
    }
  else if (chars[offset].last_count < chars[i].last_count)
    offset = i;
  }

SLJIT_ASSERT(offset == -1 || (chars[offset].count >= 1 && chars[offset].count <= 2));

if (range_right < 0)
  {
  if (offset < 0)
    return FALSE;
  /* Works regardless the value is 1 or 2. */
  fast_forward_first_char2(common, chars[offset].chars[0], chars[offset].chars[1], offset);
  return TRUE;
  }

SLJIT_ASSERT(range_right != offset);

if (common->match_end_ptr != 0)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);
  OP1(SLJIT_MOV, TMP3, 0, STR_END, 0);
  OP2(SLJIT_SUB | SLJIT_SET_LESS, STR_END, 0, STR_END, 0, SLJIT_IMM, IN_UCHARS(max));
  add_jump(compiler, &common->failed_match, JUMP(SLJIT_LESS));
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER, STR_END, 0, TMP1, 0);
  CMOV(SLJIT_GREATER, STR_END, TMP1, 0);
  }
else
  {
  OP2(SLJIT_SUB | SLJIT_SET_LESS, STR_END, 0, STR_END, 0, SLJIT_IMM, IN_UCHARS(max));
  add_jump(compiler, &common->failed_match, JUMP(SLJIT_LESS));
  }

SLJIT_ASSERT(range_right >= 0);

if (!HAS_VIRTUAL_REGISTERS)
  OP1(SLJIT_MOV, RETURN_ADDR, 0, SLJIT_IMM, (sljit_sw)update_table);

start = LABEL();
add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0));

#if PCRE2_CODE_UNIT_WIDTH == 8 || (defined SLJIT_LITTLE_ENDIAN && SLJIT_LITTLE_ENDIAN)
OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(range_right));
#else
OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(range_right + 1) - 1);
#endif

if (!HAS_VIRTUAL_REGISTERS)
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM2(RETURN_ADDR, TMP1), 0);
else
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)update_table);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0, start);

if (offset >= 0)
  {
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(offset));
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  if (chars[offset].count == 1)
    CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, chars[offset].chars[0], start);
  else
    {
    mask = chars[offset].chars[0] ^ chars[offset].chars[1];
    if (is_powerof2(mask))
      {
      OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, mask);
      CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, chars[offset].chars[0] | mask, start);
      }
    else
      {
      match = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, chars[offset].chars[0]);
      CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, chars[offset].chars[1], start);
      JUMPHERE(match);
      }
    }
  }

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
if (common->utf && offset != 0)
  {
  if (offset < 0)
    {
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    }
  else
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));

  jumpto_if_not_utf_char_start(compiler, TMP1, start);

  if (offset < 0)
    OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  }
#endif

if (offset >= 0)
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (common->match_end_ptr != 0)
  OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
else
  OP2(SLJIT_ADD, STR_END, 0, STR_END, 0, SLJIT_IMM, IN_UCHARS(max));
return TRUE;
}