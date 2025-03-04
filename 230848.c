static SLJIT_INLINE void fast_forward_newline(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_label *loop;
struct sljit_jump *lastchar;
struct sljit_jump *firstchar;
struct sljit_jump *quit;
struct sljit_jump *foundcr = NULL;
struct sljit_jump *notfoundnl;
jump_list *newline = NULL;

if (common->match_end_ptr != 0)
  {
  OP1(SLJIT_MOV, TMP3, 0, STR_END, 0);
  OP1(SLJIT_MOV, STR_END, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);
  }

if (common->nltype == NLTYPE_FIXED && common->newline > 255)
  {
  lastchar = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, str));
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
  firstchar = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP2, 0);

  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(2));
  OP2(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, SLJIT_UNUSED, 0, STR_PTR, 0, TMP1, 0);
  OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_GREATER_EQUAL);
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

  loop = LABEL();
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  quit = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
  OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
  CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff, loop);
  CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, common->newline & 0xff, loop);

  JUMPHERE(quit);
  JUMPHERE(firstchar);
  JUMPHERE(lastchar);

  if (common->match_end_ptr != 0)
    OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
  return;
  }

OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, str));
firstchar = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP2, 0);
skip_char_back(common);

loop = LABEL();
common->ff_newline_shortcut = loop;

read_char_range(common, common->nlmin, common->nlmax, TRUE);
lastchar = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
if (common->nltype == NLTYPE_ANY || common->nltype == NLTYPE_ANYCRLF)
  foundcr = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_CR);
check_newlinechar(common, common->nltype, &newline, FALSE);
set_jumps(newline, loop);

if (common->nltype == NLTYPE_ANY || common->nltype == NLTYPE_ANYCRLF)
  {
  quit = JUMP(SLJIT_JUMP);
  JUMPHERE(foundcr);
  notfoundnl = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, CHAR_NL);
  OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_EQUAL);
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  JUMPHERE(notfoundnl);
  JUMPHERE(quit);
  }
JUMPHERE(lastchar);
JUMPHERE(firstchar);

if (common->match_end_ptr != 0)
  OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
}