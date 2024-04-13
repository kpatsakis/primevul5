static void read_char7_type(compiler_common *common, BOOL full_read)
{
/* Reads the precise character type of a character into TMP1, if the character
is less than 128. Otherwise it returns with zero. Does not check STR_END. The
full_read argument tells whether characters above max are accepted or not. */
DEFINE_COMPILER;
struct sljit_jump *jump;

SLJIT_ASSERT(common->utf);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);

if (full_read)
  {
  jump = CMP(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0xc0);
  OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP2), (sljit_sw)PRIV(utf8_table4) - 0xc0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
  JUMPHERE(jump);
  }
}