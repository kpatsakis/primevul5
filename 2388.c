static void do_utfreadchar_invalid(compiler_common *common)
{
/* Slow decoding a UTF-8 character. TMP1 contains the first byte
of the character (>= 0xc0). Return char value in TMP1. STR_PTR is
undefined for invalid characters. */
DEFINE_COMPILER;
sljit_s32 i;
sljit_s32 has_cmov = sljit_has_cpu_feature(SLJIT_HAS_CMOV);
struct sljit_jump *jump;
struct sljit_jump *buffer_end_close;
struct sljit_label *three_byte_entry;
struct sljit_label *exit_invalid_label;
struct sljit_jump *exit_invalid[11];

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xc2);

/* Usually more than 3 characters remained in the subject buffer. */
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(3));

/* Not a valid start of a multi-byte sequence, no more bytes read. */
exit_invalid[0] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0xf5 - 0xc2);

buffer_end_close = CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-3));
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
/* If TMP2 is in 0x80-0xbf range, TMP1 is also increased by (0x2 << 6). */
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x80);
exit_invalid[1] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);

OP2U(SLJIT_AND | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, 0x800);
jump = JUMP(SLJIT_NOT_ZERO);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump);

/* Three-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x80);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
  CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, 0x20000);
  exit_invalid[2] = NULL;
  }
else
  exit_invalid[2] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);

OP2U(SLJIT_AND | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, 0x10000);
jump = JUMP(SLJIT_NOT_ZERO);

three_byte_entry = LABEL();

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x2d800);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x800);
  CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, INVALID_UTF_CHAR - 0xd800);
  exit_invalid[3] = NULL;
  }
else
  exit_invalid[3] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x800);
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xd800);
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP1, 0, SLJIT_IMM, 0x800);
  CMOV(SLJIT_LESS, TMP1, SLJIT_IMM, INVALID_UTF_CHAR);
  exit_invalid[4] = NULL;
  }
else
  exit_invalid[4] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x800);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump);

/* Four-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x80);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
  CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, 0);
  exit_invalid[5] = NULL;
  }
else
  exit_invalid[5] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);

OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xc10000);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x100000);
  CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, INVALID_UTF_CHAR - 0x10000);
  exit_invalid[6] = NULL;
  }
else
  exit_invalid[6] = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, 0x100000);

OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 0x10000);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(buffer_end_close);
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
exit_invalid[7] = CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0);

/* Two-byte sequence. */
OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
/* If TMP2 is in 0x80-0xbf range, TMP1 is also increased by (0x2 << 6). */
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x80);
exit_invalid[8] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);

OP2U(SLJIT_AND | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, 0x800);
jump = JUMP(SLJIT_NOT_ZERO);

OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

/* Three-byte sequence. */
JUMPHERE(jump);
exit_invalid[9] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 6);
OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, SLJIT_IMM, 0x80);
OP2(SLJIT_OR, TMP1, 0, TMP1, 0, TMP2, 0);
if (has_cmov)
  {
  OP2U(SLJIT_SUB | SLJIT_SET_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);
  CMOV(SLJIT_GREATER_EQUAL, TMP1, SLJIT_IMM, INVALID_UTF_CHAR);
  exit_invalid[10] = NULL;
  }
else
  exit_invalid[10] = CMP(SLJIT_GREATER_EQUAL, TMP2, 0, SLJIT_IMM, 0x40);

/* One will be substracted from STR_PTR later. */
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));

/* Four byte sequences are not possible. */
CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0x30000, three_byte_entry);

exit_invalid_label = LABEL();
for (i = 0; i < 11; i++)
  sljit_set_label(exit_invalid[i], exit_invalid_label);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, INVALID_UTF_CHAR);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}