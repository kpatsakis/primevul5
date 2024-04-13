static SLJIT_INLINE struct sljit_label *mainloop_entry(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_label *mainloop;
struct sljit_label *newlinelabel = NULL;
struct sljit_jump *start;
struct sljit_jump *end = NULL;
struct sljit_jump *end2 = NULL;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_label *loop;
struct sljit_jump *jump;
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */
jump_list *newline = NULL;
sljit_u32 overall_options = common->re->overall_options;
BOOL hascrorlf = (common->re->flags & PCRE2_HASCRORLF) != 0;
BOOL newlinecheck = FALSE;
BOOL readuchar = FALSE;

if (!(hascrorlf || (overall_options & PCRE2_FIRSTLINE) != 0)
    && (common->nltype == NLTYPE_ANY || common->nltype == NLTYPE_ANYCRLF || common->newline > 255))
  newlinecheck = TRUE;

SLJIT_ASSERT(common->abort_label == NULL);

if ((overall_options & PCRE2_FIRSTLINE) != 0)
  {
  /* Search for the end of the first line. */
  SLJIT_ASSERT(common->match_end_ptr != 0);
  OP1(SLJIT_MOV, TMP3, 0, STR_PTR, 0);

  if (common->nltype == NLTYPE_FIXED && common->newline > 255)
    {
    mainloop = LABEL();
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    end = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff, mainloop);
    CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, common->newline & 0xff, mainloop);
    JUMPHERE(end);
    OP2(SLJIT_SUB, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    }
  else
    {
    end = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
    mainloop = LABEL();
    /* Continual stores does not cause data dependency. */
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr, STR_PTR, 0);
    read_char(common, common->nlmin, common->nlmax, NULL, READ_CHAR_NEWLINE);
    check_newlinechar(common, common->nltype, &newline, TRUE);
    CMPTO(SLJIT_LESS, STR_PTR, 0, STR_END, 0, mainloop);
    JUMPHERE(end);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr, STR_PTR, 0);
    set_jumps(newline, LABEL());
    }

  OP1(SLJIT_MOV, STR_PTR, 0, TMP3, 0);
  }
else if ((overall_options & PCRE2_USE_OFFSET_LIMIT) != 0)
  {
  /* Check whether offset limit is set and valid. */
  SLJIT_ASSERT(common->match_end_ptr != 0);

  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, offset_limit));
    }
  else
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, offset_limit));

  OP1(SLJIT_MOV, TMP2, 0, STR_END, 0);
  end = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, (sljit_sw) PCRE2_UNSET);
  if (HAS_VIRTUAL_REGISTERS)
    OP1(SLJIT_MOV, TMP2, 0, ARGUMENTS, 0);
  else
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));

#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif /* PCRE2_CODE_UNIT_WIDTH == [16|32] */
  if (HAS_VIRTUAL_REGISTERS)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(jit_arguments, begin));

  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, TMP1, 0);
  end2 = CMP(SLJIT_LESS_EQUAL, TMP2, 0, STR_END, 0);
  OP1(SLJIT_MOV, TMP2, 0, STR_END, 0);
  JUMPHERE(end2);
  OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, PCRE2_ERROR_NOMATCH);
  add_jump(compiler, &common->abort, CMP(SLJIT_LESS, TMP2, 0, STR_PTR, 0));
  JUMPHERE(end);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr, TMP2, 0);
  }

start = JUMP(SLJIT_JUMP);

if (newlinecheck)
  {
  newlinelabel = LABEL();
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  end = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, common->newline & 0xff);
  OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_EQUAL);
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif /* PCRE2_CODE_UNIT_WIDTH == [16|32] */
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  end2 = JUMP(SLJIT_JUMP);
  }

mainloop = LABEL();

/* Increasing the STR_PTR here requires one less jump in the most common case. */
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
if (common->utf && !common->invalid_utf) readuchar = TRUE;
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */
if (newlinecheck) readuchar = TRUE;

if (readuchar)
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);

if (newlinecheck)
  CMPTO(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff, newlinelabel);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
#if PCRE2_CODE_UNIT_WIDTH == 8
if (common->invalid_utf)
  {
  /* Skip continuation code units. */
  loop = LABEL();
  jump = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x80);
  CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x40, loop);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPHERE(jump);
  }
else if (common->utf)
  {
  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  JUMPHERE(jump);
  }
#elif PCRE2_CODE_UNIT_WIDTH == 16
if (common->invalid_utf)
  {
  /* Skip continuation code units. */
  loop = LABEL();
  jump = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xdc00);
  CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x400, loop);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPHERE(jump);
  }
else if (common->utf)
  {
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);

  if (sljit_has_cpu_feature(SLJIT_HAS_CMOV))
    {
    OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x400);
    CMOV(SLJIT_LESS, STR_PTR, TMP2, 0);
    }
  else
    {
    OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x400);
    OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_LESS);
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
    }
  }
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16] */
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32 */
JUMPHERE(start);

if (newlinecheck)
  {
  JUMPHERE(end);
  JUMPHERE(end2);
  }

return mainloop;
}