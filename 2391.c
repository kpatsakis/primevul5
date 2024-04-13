static void do_utfmoveback_invalid(compiler_common *common)
{
/* Goes one character back. */
DEFINE_COMPILER;
struct sljit_jump *exit_invalid[3];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

exit_invalid[0] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x400);
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, STR_PTR, 0);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
exit_invalid[2] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x400);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 1);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(exit_invalid[0]);
JUMPHERE(exit_invalid[1]);
JUMPHERE(exit_invalid[2]);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}