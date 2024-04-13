static void fast_forward_first_char2_sse2(compiler_common *common, PCRE2_UCHAR char1, PCRE2_UCHAR char2, sljit_s32 offset)
{
DEFINE_COMPILER;
struct sljit_label *start;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
struct sljit_label *restart;
#endif
struct sljit_jump *quit;
struct sljit_jump *partial_quit[2];
sljit_u8 instruction[8];
sljit_s32 tmp1_ind = sljit_get_register_index(TMP1);
sljit_s32 str_ptr_ind = sljit_get_register_index(STR_PTR);
sljit_s32 data_ind = 0;
sljit_s32 tmp_ind = 1;
sljit_s32 cmp1_ind = 2;
sljit_s32 cmp2_ind = 3;
sljit_u32 bit = 0;

SLJIT_UNUSED_ARG(offset);

if (char1 != char2)
  {
  bit = char1 ^ char2;
  if (!is_powerof2(bit))
    bit = 0;
  }

partial_quit[0] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
if (common->mode == PCRE2_JIT_COMPLETE)
  add_jump(compiler, &common->failed_match, partial_quit[0]);

/* First part (unaligned start) */

OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(char1 | bit));

SLJIT_ASSERT(tmp1_ind < 8);

/* MOVD xmm, r/m32 */
instruction[0] = 0x66;
instruction[1] = 0x0f;
instruction[2] = 0x6e;
instruction[3] = 0xc0 | (cmp1_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 4);

if (char1 != char2)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, character_to_int32(bit != 0 ? bit : char2));

  /* MOVD xmm, r/m32 */
  instruction[3] = 0xc0 | (cmp2_ind << 3) | tmp1_ind;
  sljit_emit_op_custom(compiler, instruction, 4);
  }

OP1(SLJIT_MOV, TMP2, 0, STR_PTR, 0);

/* PSHUFD xmm1, xmm2/m128, imm8 */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0x70;
instruction[3] = 0xc0 | (cmp1_ind << 3) | 2;
instruction[4] = 0;
sljit_emit_op_custom(compiler, instruction, 5);

if (char1 != char2)
  {
  /* PSHUFD xmm1, xmm2/m128, imm8 */
  instruction[3] = 0xc0 | (cmp2_ind << 3) | 3;
  sljit_emit_op_custom(compiler, instruction, 5);
  }

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
restart = LABEL();
#endif
OP2(SLJIT_AND, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, ~0xf);
OP2(SLJIT_AND, TMP2, 0, TMP2, 0, SLJIT_IMM, 0xf);

load_from_mem_sse2(compiler, data_ind, str_ptr_ind);
fast_forward_char_pair_sse2_compare(compiler, char1, char2, bit, data_ind, cmp1_ind, cmp2_ind, tmp_ind);

/* PMOVMSKB reg, xmm */
/* instruction[0] = 0x66; */
/* instruction[1] = 0x0f; */
instruction[2] = 0xd7;
instruction[3] = 0xc0 | (tmp1_ind << 3) | 0;
sljit_emit_op_custom(compiler, instruction, 4);

OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, TMP2, 0);

/* BSF r32, r/m32 */
instruction[0] = 0x0f;
instruction[1] = 0xbc;
instruction[2] = 0xc0 | (tmp1_ind << 3) | tmp1_ind;
sljit_emit_op_custom(compiler, instruction, 3);
sljit_set_current_flags(compiler, SLJIT_SET_Z);

quit = JUMP(SLJIT_NOT_ZERO);

OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

start = LABEL();
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, 16);

partial_quit[1] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
if (common->mode == PCRE2_JIT_COMPLETE)
  add_jump(compiler, &common->failed_match, partial_quit[1]);

/* Second part (aligned) */

load_from_mem_sse2(compiler, 0, str_ptr_ind);
fast_forward_char_pair_sse2_compare(compiler, char1, char2, bit, data_ind, cmp1_ind, cmp2_ind, tmp_ind);

/* PMOVMSKB reg, xmm */
instruction[0] = 0x66;
instruction[1] = 0x0f;
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

JUMPHERE(quit);
OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);

if (common->mode != PCRE2_JIT_COMPLETE)
  {
  JUMPHERE(partial_quit[0]);
  JUMPHERE(partial_quit[1]);
  OP2(SLJIT_SUB | SLJIT_SET_GREATER, SLJIT_UNUSED, 0, STR_PTR, 0, STR_END, 0);
  CMOV(SLJIT_GREATER, STR_PTR, STR_END, 0);
  }
else
  add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
if (common->utf && offset > 0)
  {
  SLJIT_ASSERT(common->mode == PCRE2_JIT_COMPLETE);

  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-offset));

  quit = jump_if_utf_char_start(compiler, TMP1);

  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  add_jump(compiler, &common->failed_match, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));
  OP1(SLJIT_MOV, TMP2, 0, STR_PTR, 0);
  JUMPTO(SLJIT_JUMP, restart);

  JUMPHERE(quit);
  }
#endif
}