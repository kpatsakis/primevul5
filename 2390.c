static void peek_char(compiler_common *common, sljit_u32 max, sljit_s32 dst, sljit_sw dstw, jump_list **backtracks)
{
/* Reads the character into TMP1, keeps STR_PTR.
Does not check STR_END. TMP2, dst, RETURN_ADDR Destroyed. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_jump *jump;
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */

SLJIT_UNUSED_ARG(max);
SLJIT_UNUSED_ARG(dst);
SLJIT_UNUSED_ARG(dstw);
SLJIT_UNUSED_ARG(backtracks);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));

#ifdef SUPPORT_UNICODE
#if PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  if (max < 128) return;

  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x80);
  OP1(SLJIT_MOV, dst, dstw, STR_PTR, 0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  add_jump(compiler, common->invalid_utf ? &common->utfreadchar_invalid : &common->utfreadchar, JUMP(SLJIT_FAST_CALL));
  OP1(SLJIT_MOV, STR_PTR, 0, dst, dstw);
  if (backtracks && common->invalid_utf)
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
  JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  if (max < 0xd800) return;

  OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);

  if (common->invalid_utf)
    {
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800);
    OP1(SLJIT_MOV, dst, dstw, STR_PTR, 0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    add_jump(compiler, &common->utfreadchar_invalid, JUMP(SLJIT_FAST_CALL));
    OP1(SLJIT_MOV, STR_PTR, 0, dst, dstw);
    if (backtracks && common->invalid_utf)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
    }
  else
    {
    /* TMP2 contains the high surrogate. */
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000 - 0xdc00);
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
    }

  JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 32
if (common->invalid_utf)
  {
  if (max < 0xd800) return;

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