static void check_vspace(compiler_common *common)
{
/* Check whether TMP1 contains a newline character. TMP2 destroyed. */
DEFINE_COMPILER;

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x0a);
OP2(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0x0d - 0x0a);
OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS_EQUAL);
OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0x85 - 0x0a);
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
#if PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
#endif
  OP_FLAGS(SLJIT_OR, TMP2, 0, SLJIT_EQUAL);
  OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x1);
  OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0x2029 - 0x0a);
#if PCRE2_CODE_UNIT_WIDTH == 8
  }
#endif
#endif /* SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH == [16|32] */
OP_FLAGS(SLJIT_OR | SLJIT_SET_Z, TMP2, 0, SLJIT_EQUAL);

sljit_emit_fast_return(compiler, RETURN_ADDR, 0);
}