static void read_char8_type(compiler_common *common, jump_list **backtracks, BOOL negated)
{
/* Reads the character type into TMP1, updates STR_PTR. Does not check STR_END. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
struct sljit_jump *jump;
#endif
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
struct sljit_jump *jump2;
#endif

SLJIT_UNUSED_ARG(backtracks);
SLJIT_UNUSED_ARG(negated);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  /* The result of this read may be unused, but saves an "else" part. */
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
  jump = CMP(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0x80);

  if (!negated)
    {
    if (common->invalid_utf)
      add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2);
    if (common->invalid_utf)
      add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xe0 - 0xc2));

    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
    OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, TMP1, 0);
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x80);
    if (common->invalid_utf)
      add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40));

    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 255);
    OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
    JUMPHERE(jump2);
    }
  else if (common->invalid_utf)
    {
    add_jump(compiler, &common->utfreadchar_invalid, JUMP(SLJIT_FAST_CALL));
    OP1(SLJIT_MOV, TMP2, 0, TMP1, 0);
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));

    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 255);
    OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
    JUMPHERE(jump2);
    }
  else
    add_jump(compiler, &common->utfreadtype8, JUMP(SLJIT_FAST_CALL));

  JUMPHERE(jump);
  return;
  }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8 */

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 32
if (common->invalid_utf && negated)
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x110000));
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 32 */

#if PCRE2_CODE_UNIT_WIDTH != 8
/* The ctypes array contains only 256 values. */
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 255);
#endif /* PCRE2_CODE_UNIT_WIDTH != 8 */
OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
#if PCRE2_CODE_UNIT_WIDTH != 8
JUMPHERE(jump);
#endif /* PCRE2_CODE_UNIT_WIDTH != 8 */

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf && negated)
  {
  /* Skip low surrogate if necessary. */
  if (!common->invalid_utf)
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xd800);

    if (sljit_has_cpu_feature(SLJIT_HAS_CMOV) && !HAS_VIRTUAL_REGISTERS)
      {
      OP2(SLJIT_ADD, RETURN_ADDR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP2, 0, SLJIT_IMM, 0x400);
      CMOV(SLJIT_LESS, STR_PTR, RETURN_ADDR, 0);
      }
    else
      {
      jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x400);
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      JUMPHERE(jump);
      }
    return;
    }

  OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xd800);
  jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800);
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x400));
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

  OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xdc00);
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x400));

  JUMPHERE(jump);
  return;
  }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16 */
}