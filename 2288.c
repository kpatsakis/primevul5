static void read_char(compiler_common *common, sljit_u32 min, sljit_u32 max,
  jump_list **backtracks, sljit_u32 options)
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

SLJIT_UNUSED_ARG(min);
SLJIT_UNUSED_ARG(max);
SLJIT_UNUSED_ARG(backtracks);
SLJIT_UNUSED_ARG(options);
SLJIT_ASSERT(min <= max);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

#ifdef SUPPORT_UNICODE
#if PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  if (max < 128 && !(options & READ_CHAR_UPDATE_STR_PTR)) return;

  if (common->invalid_utf && !(options & READ_CHAR_VALID_UTF))
    {
    jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x80);

    if (options & READ_CHAR_UTF8_NEWLINE)
      add_jump(compiler, &common->utfreadnewline_invalid, JUMP(SLJIT_FAST_CALL));
    else
      add_jump(compiler, &common->utfreadchar_invalid, JUMP(SLJIT_FAST_CALL));

    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
    JUMPHERE(jump);
    return;
    }

  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
  if (min >= 0x10000)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xf0);
    if (options & READ_CHAR_UPDATE_STR_PTR)
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
    if (!(options & READ_CHAR_UPDATE_STR_PTR))
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump2);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  else if (min >= 0x800 && max <= 0xffff)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xe0);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP1(SLJIT_MOV_U8, RETURN_ADDR, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xf);
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    if (!(options & READ_CHAR_UPDATE_STR_PTR))
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump2);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  else if (max >= 0x800)
    {
    add_jump(compiler, &common->utfreadchar, JUMP(SLJIT_FAST_CALL));
    }
  else if (max < 128)
    {
    OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
    }
  else
    {
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (!(options & READ_CHAR_UPDATE_STR_PTR))
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    else
      OP1(SLJIT_MOV_U8, RETURN_ADDR, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, RETURN_ADDR, 0);
    }
  JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  if (max < 0xd800 && !(options & READ_CHAR_UPDATE_STR_PTR)) return;

  if (common->invalid_utf && !(options & READ_CHAR_VALID_UTF))
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800);

    if (options & READ_CHAR_UTF8_NEWLINE)
      add_jump(compiler, &common->utfreadnewline_invalid, JUMP(SLJIT_FAST_CALL));
    else
      add_jump(compiler, &common->utfreadchar_invalid, JUMP(SLJIT_FAST_CALL));

    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
    JUMPHERE(jump);
    return;
    }

  if (max >= 0x10000)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800);
    /* TMP2 contains the high surrogate. */
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000 - 0xdc00);
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
    JUMPHERE(jump);
    return;
    }

  /* Skip low surrogate if necessary. */
  OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);

  if (sljit_has_cpu_feature(SLJIT_HAS_CMOV) && !HAS_VIRTUAL_REGISTERS)
    {
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP2(SLJIT_ADD, RETURN_ADDR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP2, 0, SLJIT_IMM, 0x400);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      CMOV(SLJIT_LESS, STR_PTR, RETURN_ADDR, 0);
    if (max >= 0xd800)
      CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, 0x10000);
    }
  else
    {
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x400);
    if (options & READ_CHAR_UPDATE_STR_PTR)
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    if (max >= 0xd800)
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0x10000);
    JUMPHERE(jump);
    }
  }
#elif PCRE2_CODE_UNIT_WIDTH == 32
if (common->invalid_utf)
  {
  if (backtracks != NULL)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x110000));
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800));
    }
  else
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x110000);
    CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, INVALID_UTF_CHAR);
    OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800);
    CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, INVALID_UTF_CHAR);
    }
  }
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
#endif /* SUPPORT_UNICODE */
}