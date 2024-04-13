static void move_back(compiler_common *common, jump_list **backtracks, BOOL must_be_valid)
{
/* Goes one character back. Affects STR_PTR and TMP1. If must_be_valid is TRUE,
TMP2 is not used. Otherwise TMP2 must contain the start of the subject buffer,
and it is destroyed. Does not modify STR_PTR for invalid character sequences. */
DEFINE_COMPILER;

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_jump *jump;
#endif

#ifdef SUPPORT_UNICODE
#if PCRE2_CODE_UNIT_WIDTH == 8
struct sljit_label *label;

if (common->utf)
  {
  if (!must_be_valid && common->invalid_utf)
    {
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -IN_UCHARS(1));
    OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x80);
    add_jump(compiler, &common->utfmoveback_invalid, JUMP(SLJIT_FAST_CALL));
    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0));
    JUMPHERE(jump);
    return;
    }

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

  if (!must_be_valid && common->invalid_utf)
    {
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    jump = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xe000 - 0xd800);
    add_jump(compiler, &common->utfmoveback_invalid, JUMP(SLJIT_FAST_CALL));
    if (backtracks != NULL)
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0));
    JUMPHERE(jump);
    return;
    }

  /* Skip low surrogate if necessary. */
  OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xfc00);
  OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, 0xdc00);
  OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_EQUAL);
  OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  return;
  }
#elif PCRE2_CODE_UNIT_WIDTH == 32
if (common->invalid_utf && !must_be_valid)
  {
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -IN_UCHARS(1));
  if (backtracks != NULL)
    {
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x110000));
    OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    return;
    }

  OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x110000);
  OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_LESS);
  OP2(SLJIT_SHL,  TMP1, 0, TMP1, 0, SLJIT_IMM, UCHAR_SHIFT);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
  return;
  }
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
#endif /* SUPPORT_UNICODE */

SLJIT_UNUSED_ARG(backtracks);
SLJIT_UNUSED_ARG(must_be_valid);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
}