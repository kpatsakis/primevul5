static void do_utfreadnewline_invalid(compiler_common *common)
{
/* Slow decoding a UTF-8 character, specialized for newlines.
TMP1 contains the first byte of the character (>= 0xc0). Return
char value in TMP1. */
DEFINE_COMPILER;
struct sljit_label *loop;
struct sljit_label *skip_start;
struct sljit_label *three_byte_exit;
struct sljit_jump *jump[5];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

if (common->nltype != NLTYPE_ANY)
  {
  SLJIT_ASSERT(common->nltype != NLTYPE_FIXED || common->newline < 128);

  /* All newlines are ascii, just skip intermediate octets. */
  jump[0] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  loop = LABEL();
  if (sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_SUPP | SLJIT_MEM_POST, TMP2, SLJIT_MEM1(STR_PTR), IN_UCHARS(1)) == SLJIT_SUCCESS)
    sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_POST, TMP2, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
  else
    {
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    }

  OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc0);
  CMPTO(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, 0x80, loop);
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  JUMPHERE(jump[0]);

  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
  OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
  return;
  }

jump[0] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

jump[1] = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0xc2);
jump[2] = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0xe2);

skip_start = LABEL();
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc0);
jump[3] = CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, 0x80);

/* Skip intermediate octets. */
loop = LABEL();
jump[4] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc0);
CMPTO(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, 0x80, loop);

JUMPHERE(jump[3]);
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

three_byte_exit = LABEL();
JUMPHERE(jump[0]);
JUMPHERE(jump[4]);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Two byte long newline: 0x85. */
JUMPHERE(jump[1]);
CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, 0x85, skip_start);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0x85);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Three byte long newlines: 0x2028 and 0x2029. */
JUMPHERE(jump[2]);
CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, 0x80, skip_start);
CMPTO(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0, three_byte_exit);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

OP2(SLJIT_SUB, TMP1, 0, TMP2, 0, SLJIT_IMM, 0x80);
CMPTO(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x40, skip_start);

OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, 0x2000);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}