static void peek_char_back(compiler_common *common, sljit_u32 max, jump_list **backtracks)
{
/* Reads one character back without moving STR_PTR. TMP2 must
contain the start of the subject buffer. Affects TMP1, TMP2, and RETURN_ADDR. */
DEFINE_COMPILER;

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_jump *jump;
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */

SLJIT_UNUSED_ARG(max);
SLJIT_UNUSED_ARG(backtracks);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));

#ifdef SUPPORT_UNICODE
#if PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  if (max < 128) return;

  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x80);
  if (common->invalid_utf)
    {
    add_jump(compiler, &common->utfpeakcharback_invalid, JUMP(SLJIT_FAST_CALL));
    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
    }
  else
    add_jump(compiler, &common->utfpeakcharback, JUMP(SLJIT_FAST_CALL));
  JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  if (max < 0xd800) return;

  if (common->invalid_utf)
    {
    jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xd800);
    add_jump(compiler, &common->utfpeakcharback_invalid, JUMP(SLJIT_FAST_CALL));
    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR));
    }
  else
    {
    OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xdc00);
    jump = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0xe000 - 0xdc00);
    /* TMP2 contains the low surrogate. */
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
    OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x10000);
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 10);
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
    }
    JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 32
if (common->invalid_utf)
  {
  OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x110000));
  add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0xe000 - 0xd800));
  }
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
#endif /* SUPPORT_UNICODE */
}