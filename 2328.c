static void check_newlinechar(compiler_common *common, int nltype, jump_list **backtracks, BOOL jumpifmatch)
{
/* Character comes in TMP1. Checks if it is a newline. TMP2 may be destroyed. */
DEFINE_COMPILER;
struct sljit_jump *jump;

if (nltype == NLTYPE_ANY)
  {
  add_jump(compiler, &common->anynewline, JUMP(SLJIT_FAST_CALL));
  sljit_set_current_flags(compiler, SLJIT_SET_Z);
  add_jump(compiler, backtracks, JUMP(jumpifmatch ? SLJIT_NOT_ZERO : SLJIT_ZERO));
  }
else if (nltype == NLTYPE_ANYCRLF)
  {
  if (jumpifmatch)
    {
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_CR));
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_NL));
    }
  else
    {
    jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_CR);
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_NL));
    JUMPHERE(jump);
    }
  }
else
  {
  SLJIT_ASSERT(nltype == NLTYPE_FIXED && common->newline < 256);
  add_jump(compiler, backtracks, CMP(jumpifmatch ? SLJIT_EQUAL : SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, common->newline));
  }
}