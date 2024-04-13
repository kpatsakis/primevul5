static void do_utfreadchar(compiler_common *common)
{
/* Fast decoding a UTF-8 character. TMP1 contains the first byte
of the character (>= 0xc0). Return char value in TMP1, length in TMP2. */
DEFINE_COMPILER;
struct sljit_jump *jump;

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x3f);
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

/* Searching for the first zero. */
OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0x800);
jump = JUMP(SLJIT_NOT_ZERO);
/* Two byte sequence. */
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, IN_UCHARS(2));
sljit_emit_fast_return(compiler, RETURN_ADDR, 0);

JUMPHERE(jump);
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
OP2(SLJIT_XOR, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x800);
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0x10000);
jump = JUMP(SLJIT_NOT_ZERO);
/* Three byte sequence. */
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, IN_UCHARS(3));
sljit_emit_fast_return(compiler, RETURN_ADDR, 0);

/* Four byte sequence. */
JUMPHERE(jump);
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(2));
OP2(SLJIT_XOR, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000);
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x3f);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, IN_UCHARS(4));
sljit_emit_fast_return(compiler, RETURN_ADDR, 0);
}