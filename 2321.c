static void do_utfreadnewline_invalid(compiler_common *common)
{
/* Slow decoding a UTF-16 character, specialized for newlines.
TMP1 contains the first half of the character (>= 0xd800). Return
char value in TMP1. */

DEFINE_COMPILER;
struct sljit_jump *exit_invalid[2];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

/* TMP2 contains the high surrogate. */
exit_invalid[0] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xdc00);

OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xdc00);
OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP2, 0, SLJIT_IMM, 0x400);
OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0x10000);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, UCHAR_SHIFT);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(exit_invalid[0]);
JUMPHERE(exit_invalid[1]);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}