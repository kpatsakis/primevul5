static void fast_forward_char_pair_sse2(compiler_common *common, sljit_s32 offs1,
  PCRE2_UCHAR char1a, PCRE2_UCHAR char1b, sljit_s32 offs2, PCRE2_UCHAR char2a, PCRE2_UCHAR char2b)
{
DEFINE_COMPILER;
sljit_u32 bit1 = 0;
sljit_u32 bit2 = 0;
sljit_u32 diff = IN_UCHARS(offs1 - offs2);
sljit_s32 tmp1_ind = sljit_get_register_index(TMP1);
sljit_s32 tmp2_ind = sljit_get_register_index(TMP2);
sljit_s32 str_ptr_ind = sljit_get_register_index(STR_PTR);
sljit_s32 data1_ind = 0;
sljit_s32 data2_ind = 1;
sljit_s32 tmp_ind = 2;
sljit_s32 cmp1a_ind = 3;
sljit_s32 cmp1b_ind = 4;
sljit_s32 cmp2a_ind = 5;
sljit_s32 cmp2b_ind = 6;
struct sljit_label *start;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_label *restart;
#endif
struct sljit_jump *jump[2];

sljit_u8 instruction[8];

SLJIT_ASSERT(common->mode == PCRE2_JIT_COMPLETE && offs1 > offs2);
SLJIT_ASSERT(diff <= IN_UCHARS(max_fast_forward_char_pair_sse2_offset()));
SLJIT_ASSERT(tmp1_ind < 8 && tmp2_ind == 1);

/* Initialize. */
if (common->match_end_ptr != 0)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);
  OP1(SLJIT_MOV, TMP3, 0, STR_END, 0);
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(offs1 + 1));

  OP2(SLJIT_SUB | SLJIT_SET_LESS, SLJIT_UNUSED, 0, TMP1, 0, STR_END, 0);
  CMOV(SLJIT_LESS, STR_END, TMP1, 0);
  }

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offs1));
add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

/* MOVD xmm, r/m32 */
instruction[0] = 0x66;
instruction[1] = 0x0f;
instruction[2] = 0x6e;

if (char1a == char1b)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char1a));
else
  {
  bit1 = char1a ^ char1b;
  if (is_powerof2(bit1))
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char1a | bit1));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, character_to_int32(bit1));
    }
  else
    {
    bit1 = 0;
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char1a));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, character_to_int32(char1b));
    }
  }

instruction[3] = 0xc0 | (cmp1a_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

if (char1a != char1b)
  {
  instruction[3] = 0xc0 | (cmp1b_ind << 3) | tmp2_ind;
  sljit_emit_op_custom(compiler, instruction, 4);
  }

if (char2a == char2b)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char2a));
else
  {
  bit2 = char2a ^ char2b;
  if (is_powerof2(bit2))
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char2a | bit2));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, character_to_int32(bit2));
    }
  else
    {
    bit2 = 0;
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char2a));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, character_to_int32(char2b));
    }
  }

instruction[3] = 0xc0 | (cmp2a_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

if (char2a != char2b)
  {
  instruction[3] = 0xc0 | (cmp2b_ind << 3) | tmp2_ind;
  sljit_emit_op_custom(compiler, instruction, 4);
  }

/* PSHUFD xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x70;
instruction[4] = 0;

instruction[3] = 0xc0 | (cmp1a_ind << 3) | cmp1a_ind;
sljit_emit_op_custom(compiler, instruction, 5);

if (char1a != char1b)
  {
  instruction[3] = 0xc0 | (cmp1b_ind << 3) | cmp1b_ind;
  sljit_emit_op_custom(compiler, instruction, 5);
  }

instruction[3] = 0xc0 | (cmp2a_ind << 3) | cmp2a_ind;
sljit_emit_op_custom(compiler, instruction, 5);

if (char2a != char2b)
  {
  instruction[3] = 0xc0 | (cmp2b_ind << 3) | cmp2b_ind;
  sljit_emit_op_custom(compiler, instruction, 5);
  }

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
restart = LABEL();
#endif

OP2(SLJIT_SUB, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offs1 - offs2));
OP1(SLJIT_MOV, TMP2, 0, STR_PTR, 0);
OP2(SLJIT_AND, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, ~0xf);
OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, ~0xf);

load_from_mem_sse2(compiler, data1_ind, str_ptr_ind);

jump[0] = CMP(SLJIT_EQUAL, STR_PTR, 0, TMP1, 0);

load_from_mem_sse2(compiler, data2_ind, tmp1_ind);

/* MOVDQA xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x6f;
instruction[3] = 0xc0 | (tmp_ind << 3) | data1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

/* PSLLDQ xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x73;
instruction[3] = 0xc0 | (7 << 3) | tmp_ind;
instruction[4] = diff;
sljit_emit_op_custom(compiler, instruction, 5);

/* PSRLDQ xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
/* instruction[2] = 0x73; */
instruction[3] = 0xc0 | (3 << 3) | data2_ind;
instruction[4] = 16 - diff;
sljit_emit_op_custom(compiler, instruction, 5);

/* POR xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xeb;
instruction[3] = 0xc0 | (data2_ind << 3) | tmp_ind;
sljit_emit_op_custom(compiler, instruction, 4);

jump[1] = JUMP(SLJIT_JUMP);

JUMPHERE(jump[0]);

/* MOVDQA xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x6f;
instruction[3] = 0xc0 | (data2_ind << 3) | data1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

/* PSLLDQ xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x73;
instruction[3] = 0xc0 | (7 << 3) | data2_ind;
instruction[4] = diff;
sljit_emit_op_custom(compiler, instruction, 5);

JUMPHERE(jump[1]);

OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xf);

fast_forward_char_pair_sse2_compare(compiler, char2a, char2b, bit2, data2_ind, cmp2a_ind, cmp2b_ind, tmp_ind);
fast_forward_char_pair_sse2_compare(compiler, char1a, char1b, bit1, data1_ind, cmp1a_ind, cmp1b_ind, tmp_ind);

/* PAND xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xdb;
instruction[3] = 0xc0 | (data1_ind << 3) | data2_ind;
sljit_emit_op_custom(compiler, instruction, 4);

/* PMOVMSKB reg, xmm */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xd7;
instruction[3] = 0xc0 | (tmp1_ind << 3) | 0;
sljit_emit_op_custom(compiler, instruction, 4);

/* Ignore matches before the first STR_PTR. */
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, TMP2, 0);

/* BSF r32, r/m32 */
instruction[0] = 0x0f;
instruction[1] = 0xbc;
instruction[2] = 0xc0 | (tmp1_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 3);
sljit_set_current_flags(compiler, SLJIT_SET_Z);

jump[0] = JUMP(SLJIT_NOT_ZERO);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

/* Main loop. */
instruction[0] = 0x66;
instruction[1] = 0x0f;

start = LABEL();

load_from_mem_sse2(compiler, data2_ind, str_ptr_ind);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, 16);
add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

load_from_mem_sse2(compiler, data1_ind, str_ptr_ind);

/* PSRLDQ xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x73;
instruction[3] = 0xc0 | (3 << 3) | data2_ind;
instruction[4] = 16 - diff;
sljit_emit_op_custom(compiler, instruction, 5);

/* MOVDQA xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x6f;
instruction[3] = 0xc0 | (tmp_ind << 3) | data1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

/* PSLLDQ xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x73;
instruction[3] = 0xc0 | (7 << 3) | tmp_ind;
instruction[4] = diff;
sljit_emit_op_custom(compiler, instruction, 5);

/* POR xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xeb;
instruction[3] = 0xc0 | (data2_ind << 3) | tmp_ind;
sljit_emit_op_custom(compiler, instruction, 4);

fast_forward_char_pair_sse2_compare(compiler, char1a, char1b, bit1, data1_ind, cmp1a_ind, cmp1b_ind, tmp_ind);
fast_forward_char_pair_sse2_compare(compiler, char2a, char2b, bit2, data2_ind, cmp2a_ind, cmp2b_ind, tmp_ind);

/* PAND xmm1, xmm2/m128 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xdb;
instruction[3] = 0xc0 | (data1_ind << 3) | data2_ind;
sljit_emit_op_custom(compiler, instruction, 4);

/* PMOVMSKB reg, xmm */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xd7;
instruction[3] = 0xc0 | (tmp1_ind << 3) | 0;
sljit_emit_op_custom(compiler, instruction, 4);

/* BSF r32, r/m32 */
instruction[0] = 0x0f;
instruction[1] = 0xbc;
instruction[2] = 0xc0 | (tmp1_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 3);
sljit_set_current_flags(compiler, SLJIT_SET_Z);

JUMPTO(SLJIT_ZERO, start);

JUMPHERE(jump[0]);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);

add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

if (common->match_end_ptr != 0)
  OP1(SLJIT_MOV, STR_END, 0, SLJIT_MEM1(SLJIT_SP), common->match_end_ptr);

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
if (common->utf)
  {
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-offs1));

  jump[0] = jump_if_utf_char_start(compiler, TMP1);

  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  CMPTO(SLJIT_LESS, STR_PTR, 0, STR_END, 0, restart);

  add_jump(compiler, &common->failed_match, JUMP(SLJIT_JUMP));

  JUMPHERE(jump[0]);
  }
#endif

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(offs1));

if (common->match_end_ptr != 0)
  OP1(SLJIT_MOV, STR_END, 0, TMP3, 0);
}