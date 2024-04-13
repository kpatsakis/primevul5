static void do_utfmoveback_invalid(compiler_common *common)
{
/* Goes one character back. */
DEFINE_COMPILER;
sljit_s32 i;
struct sljit_jump *jump;
struct sljit_jump *buffer_start_close;
struct sljit_label *exit_ok_label;
struct sljit_label *exit_invalid_label;
struct sljit_jump *exit_invalid[7];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));
exit_invalid[0] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xc0);

/* Two-byte sequence. */
buffer_start_close = CMP(SLJIT_LESS, STR_PTR, 0, TMP2, 0);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(2));

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xc0);
jump = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x20);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 1);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Three-byte sequence. */
JUMPHERE(jump);
exit_invalid[1] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, -0x40);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xe0);
jump = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x10);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 1);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Four-byte sequence. */
JUMPHERE(jump);
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xe0 - 0x80);
exit_invalid[2] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x40);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xf0);
exit_invalid[3] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x05);

exit_ok_label = LABEL();
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 1);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Two-byte sequence. */
JUMPHERE(buffer_start_close);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));

exit_invalid[4] = CMP(SLJIT_LESS, STR_PTR, 0, TMP2, 0);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xc0);
CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x20, exit_ok_label);

/* Three-byte sequence. */
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
exit_invalid[5] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, -0x40);
exit_invalid[6] = CMP(SLJIT_LESS, STR_PTR, 0, TMP2, 0);

OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xe0);
CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x10, exit_ok_label);

/* Four-byte sequences are not possible. */

exit_invalid_label = LABEL();
sljit_set_label(exit_invalid[5], exit_invalid_label);
sljit_set_label(exit_invalid[6], exit_invalid_label);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(exit_invalid[4]);
/* -2 + 4 = 2 */
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));

exit_invalid_label = LABEL();
for (i = 0; i < 4; i++)
  sljit_set_label(exit_invalid[i], exit_invalid_label);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(4));
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}