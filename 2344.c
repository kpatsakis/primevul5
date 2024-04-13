static void do_utfreadtype8(compiler_common *common)
{
/* Fast decoding a UTF-8 character type. TMP2 contains the first byte
of the character (>= 0xc0). Return value in TMP1. */
DEFINE_COMPILER;
struct sljit_jump *jump;
struct sljit_jump *compare;

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

OP2U(SLJIT_AND | SLJIT_SET_Z, TMP2, 0, SLJIT_IMM, 0x20);
jump = JUMP(SLJIT_NOT_ZERO);
/* Two byte sequence. */
OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x1f);
/* The upper 5 bits are known at this point. */
compare = CMP(SLJIT_GREATER, TMP2, 0, SLJIT_IMM, 0x3);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
OP2(SLJIT_OR, TMP2, 0, TMP2, 0, TMP1, 0);
OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP2), common->ctypes);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(compare);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* We only have types for characters less than 256. */
JUMPHERE(jump);
OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP2), (sljit_sw)PRIV(utf8_table4) - 0xc0);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}