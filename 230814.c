static void skip_char_back(compiler_common *common)
{
/* Goes one character back. Affects STR_PTR and TMP1. Does not check begin. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
#if PCRE2_CODE_UNIT_WIDTH == 8
struct sljit_label *label;

if (common->utf)
  {
  label = LABEL();
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -IN_UCHARS(1));
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xc0);
  CMPTO(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0x80, label);
  return;
  }
#elif PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf)
  {
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -IN_UCHARS(1));
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  /* Skip low surrogate if necessary. */
  OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xfc00);
  OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0xdc00);
  OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_EQUAL);
  OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  return;
  }
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16] */
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
}