static void do_utfreadchar_invalid(compiler_common *common)
{
/* Slow decoding a UTF-16 character. TMP1 contains the first half
of the character (>= 0xd800). Return char value in TMP1. STR_PTR is
undefined for invalid characters. */
DEFINE_COMPILER;
struct sljit_jump *exit_invalid[3];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

/* TMP2 contains the high surrogate. */
exit_invalid[0] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xdc00);
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xdc00);
OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x10000);
exit_invalid[2] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x400);

OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(exit_invalid[0]);
JUMPHERE(exit_invalid[1]);
JUMPHERE(exit_invalid[2]);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}