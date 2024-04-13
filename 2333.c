static void do_utfpeakcharback_invalid(compiler_common *common)
{
/* Peak a character back. Does not modify STR_PTR. */
DEFINE_COMPILER;
struct sljit_jump *jump;
struct sljit_jump *exit_invalid[3];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

jump = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xe000);
OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
exit_invalid[0] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xdc00);
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, STR_PTR, 0);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000 - 0xdc00);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xd800);
exit_invalid[2] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x400);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 10);
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);

JUMPHERE(jump);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(exit_invalid[0]);
JUMPHERE(exit_invalid[1]);
JUMPHERE(exit_invalid[2]);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}