static void peek_char(compiler_common *common, sljit_u32 max)
{
/* Reads the character into TMP1, keeps STR_PTR.
Does not check STR_END. TMP2 Destroyed. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_jump *jump;
#endif

SLJIT_UNUSED_ARG(max);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  if (max < 128) return;

  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  add_jump(compiler, &common->utfreadchar, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
  JUMPHERE(jump);
  }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  if (max < 0xd800) return;

  OP2(SLJIT_SUB, TMP2, 0, TMP1, 0, SLJIT_IMM, 0xd800);
  jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800 - 1);
  /* TMP2 contains the high surrogate. */
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x40);
  OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
  OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3ff);
  OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
  JUMPHERE(jump);
  }
#endif
}