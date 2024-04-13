static void read_char_range(compiler_common *common, sljit_u32 min, sljit_u32 max, BOOL update_str_ptr)
{
/* Reads the precise value of a character into TMP1, if the character is
between min and max (c >= min && c <= max). Otherwise it returns with a value
outside the range. Does not check STR_END. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_jump *jump;
#endif
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
struct sljit_jump *jump2;
#endif

SLJIT_UNUSED_ARG(update_str_ptr);
SLJIT_UNUSED_ARG(min);
SLJIT_UNUSED_ARG(max);
SLJIT_ASSERT(min <= max);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  if (max < 128 && !update_str_ptr) return;

  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
  if (min >= 0x10000)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xf0);
    if (update_str_ptr)
      OP1(SLJIT_MOV_U8, RETURN_ADDR, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0x7);
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(2));
    if (!update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump2);
    if (update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  else if (min >= 0x800 && max <= 0xffff)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xe0);
    if (update_str_ptr)
      OP1(SLJIT_MOV_U8, RETURN_ADDR, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xf);
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    if (!update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump2);
    if (update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  else if (max >= 0x800)
    add_jump(compiler, (max < 0x10000) ? &common->utfreadchar16 : &common->utfreadchar, JUMP(SLJIT_FAST_CALL));
  else if (max < 128)
    {
    OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
    }
  else
    {
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (!update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    else
      OP1(SLJIT_MOV_U8, RETURN_ADDR, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    if (update_str_ptr)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  JUMPHERE(jump);
  }
#endif

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  if (max >= 0x10000)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800 - 1);
    /* TMP2 contains the high surrogate. */
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x40);
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3ff);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump);
    return;
    }

  if (max < 0xd800 && !update_str_ptr) return;

  /* Skip low surrogate if necessary. */
  OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
  jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800 - 1);
  if (update_str_ptr)
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  if (max >= 0xd800)
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0x10000);
  JUMPHERE(jump);
  }
#endif
}