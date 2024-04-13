static SLJIT_INLINE void return_with_partial_match(compiler_common *common, struct sljit_label *quit)
{
DEFINE_COMPILER;
sljit_s32 mov_opcode;
sljit_s32 arguments_reg = !HAS_VIRTUAL_REGISTERS ? ARGUMENTS : SLJIT_R1;

SLJIT_COMPILE_ASSERT(STR_END == SLJIT_S0, str_end_must_be_saved_reg0);
SLJIT_ASSERT(common->start_used_ptr != 0 && common->start_ptr != 0
  && (common->mode == PCRE2_JIT_PARTIAL_SOFT ? common->hit_start != 0 : common->hit_start == 0));

if (arguments_reg != ARGUMENTS)
  OP1(SLJIT_MOV, arguments_reg, 0, ARGUMENTS, 0);
OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_SP),
  common->mode == PCRE2_JIT_PARTIAL_SOFT ? common->hit_start : common->start_ptr);
OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, PCRE2_ERROR_PARTIAL);

/* Store match begin and end. */
OP1(SLJIT_MOV, SLJIT_S1, 0, SLJIT_MEM1(arguments_reg), SLJIT_OFFSETOF(jit_arguments, begin));
OP1(SLJIT_MOV, SLJIT_MEM1(arguments_reg), SLJIT_OFFSETOF(jit_arguments, startchar_ptr), SLJIT_R2, 0);
OP1(SLJIT_MOV, SLJIT_R1, 0, SLJIT_MEM1(arguments_reg), SLJIT_OFFSETOF(jit_arguments, match_data));

mov_opcode = (sizeof(PCRE2_SIZE) == 4) ? SLJIT_MOV_U32 : SLJIT_MOV;

OP2(SLJIT_SUB, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_S1, 0);
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
OP2(SLJIT_ASHR, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif
OP1(mov_opcode, SLJIT_MEM1(SLJIT_R1), SLJIT_OFFSETOF(pcre2_match_data, ovector), SLJIT_R2, 0);

OP2(SLJIT_SUB, STR_END, 0, STR_END, 0, SLJIT_S1, 0);
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
OP2(SLJIT_ASHR, STR_END, 0, STR_END, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif
OP1(mov_opcode, SLJIT_MEM1(SLJIT_R1), SLJIT_OFFSETOF(pcre2_match_data, ovector) + sizeof(PCRE2_SIZE), STR_END, 0);

JUMPTO(SLJIT_JUMP, quit);
}