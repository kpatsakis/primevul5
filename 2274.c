static void fast_forward_first_char2(compiler_common *common, PCRE2_UCHAR char1, PCRE2_UCHAR char2, sljit_s32 offset)
{
DEFINE_COMPILER;
struct sljit_label *start;
struct sljit_jump *match;
struct sljit_jump *partial_quit;
PCRE2_UCHAR mask;
BOOL has_match_end = (common->match_end_ptr != 0);

SLJIT_ASSERT(common->mode == PCRE2_JIT_COMPLETE || offset == 0);

if (has_match_end)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);

if (offset > 0)
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offset));

if (has_match_end)
  {
  OP1(SLJIT_MOV, TMP3, 0, STR_END, 0);

  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(offset + 1));
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER, STR_END, 0, TMP1, 0);
  CMOV(SLJIT_GREATER, STR_END, TMP1, 0);
  }

#ifdef JIT_HAS_FAST_FORWARD_CHAR_SIMD

if (JIT_HAS_FAST_FORWARD_CHAR_SIMD)
  {
  fast_forward_char_simd(common, char1, char2, offset);

  if (offset > 0)
    OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offset));

  if (has_match_end)
    OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
  return;
  }

#endif

start = LABEL();

partial_quit = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
if (common->mode == PCRE2_JIT_COMPLETE)
  add_jump(compiler, &common->failed_match, partial_quit);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (char1 == char2)
  CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, char1, start);
else
  {
  mask = char1 ^ char2;
  if (is_powerof2(mask))
    {
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, mask);
    CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, char1 | mask, start);
    }
  else
    {
    match = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, char1);
    CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, char2, start);
    JUMPHERE(match);
    }
  }

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
if (common->utf && offset > 0)
  {
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-(offset + 1)));
  jumpto_if_not_utf_char_start(compiler, TMP1, start);
  }
#endif

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offset + 1));

if (common->mode != PCRE2_JIT_COMPLETE)
  JUMPHERE(partial_quit);

if (has_match_end)
  OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
}