static void check_wordboundary(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *skipread;
jump_list *skipread_list = NULL;
#ifdef SUPPORT_UNICODE
struct sljit_label *valid_utf;
jump_list *invalid_utf1 = NULL;
#endif /* SUPPORT_UNICODE */
jump_list *invalid_utf2 = NULL;
#if PCRE2_CODE_UNIT_WIDTH != 8 || defined SUPPORT_UNICODE
struct sljit_jump *jump;
#endif /* PCRE2_CODE_UNIT_WIDTH != 8 || SUPPORT_UNICODE */

SLJIT_COMPILE_ASSERT(ctype_word == 0x10, ctype_word_must_be_16);

sljit_emit_fast_enter(compiler, SLJIT_MEM1(SLJIT_SP), LOCALS0);
/* Get type of the previous char, and put it to TMP3. */
OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
OP1(SLJIT_MOV, TMP3, 0, SLJIT_IMM, 0);
skipread = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP2, 0);

#ifdef SUPPORT_UNICODE
if (common->invalid_utf)
  {
  peek_char_back(common, READ_CHAR_MAX, &invalid_utf1);

  if (common->mode != PCRE2_JIT_COMPLETE)
    {
    OP1(SLJIT_MOV, RETURN_ADDR, 0, TMP1, 0);
    OP1(SLJIT_MOV, TMP2, 0, STR_PTR, 0);
    move_back(common, NULL, TRUE);
    check_start_used_ptr(common);
    OP1(SLJIT_MOV, TMP1, 0, RETURN_ADDR, 0);
    OP1(SLJIT_MOV, STR_PTR, 0, TMP2, 0);
    }
  }
else
#endif /* SUPPORT_UNICODE */
  {
  if (common->mode == PCRE2_JIT_COMPLETE)
    peek_char_back(common, READ_CHAR_MAX, NULL);
  else
    {
    move_back(common, NULL, TRUE);
    check_start_used_ptr(common);
    read_char(common, 0, READ_CHAR_MAX, NULL, READ_CHAR_UPDATE_STR_PTR);
    }
  }

/* Testing char type. */
#ifdef SUPPORT_UNICODE
if (common->ucp)
  {
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 1);
  jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_UNDERSCORE);
  add_jump(compiler, &common->getucdtype, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Ll);
  OP2U(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, TMP1, 0, SLJIT_IMM, ucp_Lu - ucp_Ll);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS_EQUAL);
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Nd - ucp_Ll);
  OP2U(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, TMP1, 0, SLJIT_IMM, ucp_No - ucp_Nd);
  OP_FLAGS(SLJIT_OR, TMP2, 0, SLJIT_LESS_EQUAL);
  JUMPHERE(jump);
  OP1(SLJIT_MOV, TMP3, 0, TMP2, 0);
  }
else
#endif /* SUPPORT_UNICODE */
  {
#if PCRE2_CODE_UNIT_WIDTH != 8
  jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#elif defined SUPPORT_UNICODE
  /* Here TMP3 has already been zeroed. */
  jump = NULL;
  if (common->utf)
    jump = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), common->ctypes);
  OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, SLJIT_IMM, 4 /* ctype_word */);
  OP2(SLJIT_AND, TMP3, 0, TMP1, 0, SLJIT_IMM, 1);
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
peek_char(common, READ_CHAR_MAX, SLJIT_MEM1(SLJIT_SP), LOCALS1, &invalid_utf2);

/* Testing char type. This is a code duplication. */
#ifdef SUPPORT_UNICODE

valid_utf = LABEL();

if (common->ucp)
  {
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 1);
  jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_UNDERSCORE);
  add_jump(compiler, &common->getucdtype, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Ll);
  OP2U(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, TMP1, 0, SLJIT_IMM, ucp_Lu - ucp_Ll);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS_EQUAL);
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, ucp_Nd - ucp_Ll);
  OP2U(SLJIT_SUB | SLJIT_SET_LESS_EQUAL, TMP1, 0, SLJIT_IMM, ucp_No - ucp_Nd);
  OP_FLAGS(SLJIT_OR, TMP2, 0, SLJIT_LESS_EQUAL);
  JUMPHERE(jump);
  }
else
#endif /* SUPPORT_UNICODE */
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

OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
OP2(SLJIT_XOR | SLJIT_SET_Z, TMP2, 0, TMP2, 0, TMP3, 0);
OP_SRC(SLJIT_FAST_RETURN, TMP1, 0);

#ifdef SUPPORT_UNICODE
if (common->invalid_utf)
  {
  set_jumps(invalid_utf1, LABEL());

  peek_char(common, READ_CHAR_MAX, SLJIT_MEM1(SLJIT_SP), LOCALS1, NULL);
  CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR, valid_utf);

  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, -1);
  OP_SRC(SLJIT_FAST_RETURN, TMP1, 0);

  set_jumps(invalid_utf2, LABEL());
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
  OP1(SLJIT_MOV, TMP2, 0, TMP3, 0);
  OP_SRC(SLJIT_FAST_RETURN, TMP1, 0);
  }
#endif /* SUPPORT_UNICODE */
}