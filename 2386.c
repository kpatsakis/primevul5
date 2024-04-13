static SLJIT_INLINE void fast_forward_start_bits(compiler_common *common)
{
DEFINE_COMPILER;
const sljit_u8 *start_bits = common->re->start_bitmap;
struct sljit_label *start;
struct sljit_jump *partial_quit;
#if PCRE2_CODE_UNIT_WIDTH != 8
struct sljit_jump *found = NULL;
#endif
jump_list *matches = NULL;

if (common->match_end_ptr != 0)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);
  OP1(SLJIT_MOV, RETURN_ADDR, 0, STR_END, 0);
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER, STR_END, 0, TMP1, 0);
  CMOV(SLJIT_GREATER, STR_END, TMP1, 0);
  }

start = LABEL();

partial_quit = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
if (common->mode == PCRE2_JIT_COMPLETE)
  add_jump(compiler, &common->failed_match, partial_quit);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (!optimize_class(common, start_bits, (start_bits[31] & 0x80) != 0, FALSE, &matches))
  {
#if PCRE2_CODE_UNIT_WIDTH != 8
  if ((start_bits[31] & 0x80) != 0)
    found = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 255);
  else
    CMPTO(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 255, start);
#elif defined SUPPORT_UNICODE
  if (common->utf && is_char7_bitset(start_bits, FALSE))
    CMPTO(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 127, start);
#endif
  OP2(SLJIT_AND, TMP2, 0, TMP1, 0, SLJIT_IMM, 0x7);
  OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, SLJIT_IMM, 3);
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)start_bits);
  if (!HAS_VIRTUAL_REGISTERS)
    {
    OP2(SLJIT_SHL, TMP3, 0, SLJIT_IMM, 1, TMP2, 0);
    OP2U(SLJIT_AND | SLJIT_SET_Z, TMP1, 0, TMP3, 0);
    }
  else
    {
    OP2(SLJIT_SHL, TMP2, 0, SLJIT_IMM, 1, TMP2, 0);
    OP2U(SLJIT_AND | SLJIT_SET_Z, TMP1, 0, TMP2, 0);
    }
  JUMPTO(SLJIT_ZERO, start);
  }
else
  set_jumps(matches, start);

#if PCRE2_CODE_UNIT_WIDTH != 8
if (found != NULL)
  JUMPHERE(found);
#endif

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (common->mode != PCRE2_JIT_COMPLETE)
  JUMPHERE(partial_quit);

if (common->match_end_ptr != 0)
  OP1(SLJIT_MOV, STR_END, 0, RETURN_ADDR, 0);
}