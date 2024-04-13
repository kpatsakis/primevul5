static void do_utfpeakcharback_invalid(compiler_common *common)
{
/* Peak a character back. Does not modify STR_PTR. */
DEFINE_COMPILER;
sljit_s32 i;
sljit_s32 has_cmov = sljit_has_cpu_feature(SLJIT_HAS_CMOV);
struct sljit_jump *jump[2];
struct sljit_label *two_byte_entry;
struct sljit_label *three_byte_entry;
struct sljit_label *exit_invalid_label;
struct sljit_jump *exit_invalid[8];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(3));
exit_invalid[0] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xc0);
jump[0] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, STR_PTR, 0);

/* Two-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2);
jump[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x1e);

two_byte_entry = LABEL();
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
/* If TMP1 is in 0x80-0xbf range, TMP1 is also increased by (0x2 << 6). */
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump[1]);
OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2 - 0x80);
OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x80);
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

/* Three-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-3));
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xe0);
jump[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x10);

three_byte_entry = LABEL();
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 12);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x800);
  CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, -0xd800);
  exit_invalid[2] = NULL;
  }
else
  exit_invalid[2] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x800);

OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x800);
  CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, INVALID_UTF_CHAR);
  exit_invalid[3] = NULL;
  }
else
  exit_invalid[3] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x800);

OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump[1]);
OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xe0 - 0x80);
exit_invalid[4] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 12);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

/* Four-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-4));
OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xf0);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 18);
/* ADD is used instead of OR because of the SUB 0x10000 above. */
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);

if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x100000);
  CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, INVALID_UTF_CHAR - 0x10000);
  exit_invalid[5] = NULL;
  }
else
  exit_invalid[5] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x100000);

OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump[0]);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
jump[0] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, STR_PTR, 0);

/* Two-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2);
CMPTO(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0x1e, two_byte_entry);

OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2 - 0x80);
OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x80);
exit_invalid[6] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 6);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);

/* Three-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-3));
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xe0);
CMPTO(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0x10, three_byte_entry);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump[0]);
exit_invalid[7] = CMP(SLJIT_GREATER, TMP2, 0, STR_PTR, 0);

/* Two-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xc2);
CMPTO(SLJIT_LESS, TMP2, 0, SLJIT_IMM, 0x1e, two_byte_entry);

exit_invalid_label = LABEL();
for (i = 0; i < 8; i++)
  sljit_set_label(exit_invalid[i], exit_invalid_label);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}