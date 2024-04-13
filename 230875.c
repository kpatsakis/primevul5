static void read_char8_type(compiler_common *common, BOOL update_str_ptr)
{
/* Reads the character type into TMP1, updates STR_PTR. Does not check STR_END. */
DEFINE_COMPILER;
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
struct sljit_jump *jump;
#endif
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
struct sljit_jump *jump2;
#endif

SLJIT_UNUSED_ARG(update_str_ptr);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
if (common->utf)
  {
  /* This can be an extra read in some situations, but hopefully
  it is needed in most cases. */
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
  jump = CMP(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0xc0);
  if (!update_str_ptr)
    {
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
    OP2(SLJIT_OR, TMP2, 0, TMP2, 0, TMP1, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
    jump2 = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 255);
    OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
    JUMPHERE(jump2);
    }
  else
    add_jump(compiler, &common->utfreadtype8, JUMP(SLJIT_FAST_CALL));
  JUMPHERE(jump);
  return;
  }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8 */

#if PCRE2_CODE_UNIT_WIDTH != 8
/* The ctypes array contains only 256 values. */
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 255);
#endif
OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
#if PCRE2_CODE_UNIT_WIDTH != 8
JUMPHERE(jump);
#endif

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16
if (common->utf && update_str_ptr)
  {
  /* Skip low surrogate if necessary. */
  OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xd800);
  jump = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0xdc00 - 0xd800 - 1);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPHERE(jump);
  }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16 */
}