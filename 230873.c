static void check_wordboundary(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *skipread;
jump_list *skipread_list = NULL;
#if PCRE2_CODE_UNIT_WIDTH != 8 || defined SUPPORT_UNICODE
struct sljit_jump *jump;
#endif

SLJIT_COMPILE_ASSERT(ctype_word == 0x10, ctype_word_must_be_16);

sljit_emit_fast_enter(compiler, SLJIT_MEM1(SLJIT_SP), LOCALS0);
/* Get type of the previous char, and put it to LOCALS1. */
OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS1, SLJIT_IMM, 0);
skipread = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP1, 0);
skip_char_back(common);
check_start_used_ptr(common);
read_char(common);

/* Testing char type. */
#ifdef SUPPORT_UNICODE
if (common->use_ucp)
  {
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 1);
  jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_UNDERSCORE);
  add_jump(compiler, &common->getucd, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Ll);
  OP2(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ucp_Lu - ucp_Ll);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS_EQUAL);
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Nd - ucp_Ll);
  OP2(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ucp_No - ucp_Nd);
  OP_FLAGS(SLJIT_OR, TMP2, 0, SLJIT_LESS_EQUAL);
  JUMPHERE(jump);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS1, TMP2, 0);
  }
else
#endif
  {
#if PCRE2_CODE_UNIT_WIDTH != 8
  jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#elif defined SUPPORT_UNICODE
  /* Here LOCALS1 has already been zeroed. */
  jump = NULL;
  if (common->utf)
    jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), common->ctypes);
  OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, SLJIT_IMM, 4 /* ctype_word */);
  OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS1, TMP1, 0);
#if PCRE2_CODE_UNIT_WIDTH != 8
  JUMPHERE(jump);
#elif defined SUPPORT_UNICODE
  if (jump != NULL)
    JUMPHERE(jump);
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
  }
JUMPHERE(skipread);

OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 0);
check_str_end(common, &skipread_list);
peek_char(common, READ_CHAR_MAX);

/* Testing char type. This is a code duplication. */
#ifdef SUPPORT_UNICODE
if (common->use_ucp)
  {
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 1);
  jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_UNDERSCORE);
  add_jump(compiler, &common->getucd, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Ll);
  OP2(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ucp_Lu - ucp_Ll);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS_EQUAL);
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Nd - ucp_Ll);
  OP2(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ucp_No - ucp_Nd);
  OP_FLAGS(SLJIT_OR, TMP2, 0, SLJIT_LESS_EQUAL);
  JUMPHERE(jump);
  }
else
#endif
  {
#if PCRE2_CODE_UNIT_WIDTH != 8
  /* TMP2 may be destroyed by peek_char. */
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 0);
  jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#elif defined SUPPORT_UNICODE
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 0);
  jump = NULL;
  if (common->utf)
    jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#endif
  OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP1), common->ctypes);
  OP2(SLJIT_LSHR, TMP2, 0, TMP2, 0, SLJIT_IMM, 4 /* ctype_word */);
  OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 1);
#if PCRE2_CODE_UNIT_WIDTH != 8
  JUMPHERE(jump);
#elif defined SUPPORT_UNICODE
  if (jump != NULL)
    JUMPHERE(jump);
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
  }
set_jumps(skipread_list, LABEL());

OP2(SLJIT_XOR | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP2, 0, SLJIT_MEM1(SLJIT_SP), LOCALS1);
sljit_emit_fast_return(compiler, SLJIT_MEM1(SLJIT_SP), LOCALS0);
}