static SLJIT_INLINE void copy_ovector(compiler_common *common, int topbracket)
{
DEFINE_COMPILER;
struct sljit_label *loop;
BOOL has_pre;

/* At this point we can freely use all registers. */
OP1(SLJIT_MOV, SLJIT_S2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1));
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(1), STR_PTR, 0);

if (HAS_VIRTUAL_REGISTERS)
  {
  OP1(SLJIT_MOV, SLJIT_R0, 0, ARGUMENTS, 0);
  OP1(SLJIT_MOV, SLJIT_S0, 0, SLJIT_MEM1(SLJIT_SP), common->start_ptr);
  if (common->mark_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_SP), common->mark_ptr);
  OP1(SLJIT_MOV_U32, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_R0), SLJIT_OFFSETOF(jit_arguments, oveccount));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), SLJIT_OFFSETOF(jit_arguments, startchar_ptr), SLJIT_S0, 0);
  if (common->mark_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), SLJIT_OFFSETOF(jit_arguments, mark_ptr), SLJIT_R2, 0);
  OP2(SLJIT_ADD, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_R0), SLJIT_OFFSETOF(jit_arguments, match_data),
    SLJIT_IMM, SLJIT_OFFSETOF(pcre2_match_data, ovector) - sizeof(PCRE2_SIZE));
  }
else
  {
  OP1(SLJIT_MOV, SLJIT_S0, 0, SLJIT_MEM1(SLJIT_SP), common->start_ptr);
  OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, match_data));
  if (common->mark_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_SP), common->mark_ptr);
  OP1(SLJIT_MOV_U32, SLJIT_R1, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, oveccount));
  OP1(SLJIT_MOV, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, startchar_ptr), SLJIT_S0, 0);
  if (common->mark_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, mark_ptr), SLJIT_R0, 0);
  OP2(SLJIT_ADD, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, SLJIT_OFFSETOF(pcre2_match_data, ovector) - sizeof(PCRE2_SIZE));
  }

has_pre = sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_SUPP | SLJIT_MEM_PRE, SLJIT_S1, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw)) == SLJIT_SUCCESS;

GET_LOCAL_BASE(SLJIT_S0, 0, OVECTOR_START - (has_pre ? sizeof(sljit_sw) : 0));
OP1(SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(HAS_VIRTUAL_REGISTERS ? SLJIT_R0 : ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));

loop = LABEL();

if (has_pre)
  sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_PRE, SLJIT_S1, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw));
else
  {
  OP1(SLJIT_MOV, SLJIT_S1, 0, SLJIT_MEM1(SLJIT_S0), 0);
  OP2(SLJIT_ADD, SLJIT_S0, 0, SLJIT_S0, 0, SLJIT_IMM, sizeof(sljit_sw));
  }

OP2(SLJIT_ADD, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, sizeof(PCRE2_SIZE));
OP2(SLJIT_SUB, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_R0, 0);
/* Copy the integer value to the output buffer */
#if PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
OP2(SLJIT_ASHR, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_IMM, UCHAR_SHIFT);
#endif

SLJIT_ASSERT(sizeof(PCRE2_SIZE) == 4 || sizeof(PCRE2_SIZE) == 8);
OP1(((sizeof(PCRE2_SIZE) == 4) ? SLJIT_MOV_U32 : SLJIT_MOV), SLJIT_MEM1(SLJIT_R2), 0, SLJIT_S1, 0);

OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
JUMPTO(SLJIT_NOT_ZERO, loop);

/* Calculate the return value, which is the maximum ovector value. */
if (topbracket > 1)
  {
  if (sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_SUPP | SLJIT_MEM_PRE, SLJIT_R2, SLJIT_MEM1(SLJIT_R0), -(2 * (sljit_sw)sizeof(sljit_sw))) == SLJIT_SUCCESS)
    {
    GET_LOCAL_BASE(SLJIT_R0, 0, OVECTOR_START + topbracket * 2 * sizeof(sljit_sw));
    OP1(SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, topbracket + 1);

    /* OVECTOR(0) is never equal to SLJIT_S2. */
    loop = LABEL();
    sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_PRE, SLJIT_R2, SLJIT_MEM1(SLJIT_R0), -(2 * (sljit_sw)sizeof(sljit_sw)));
    OP2(SLJIT_SUB, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
    CMPTO(SLJIT_EQUAL, SLJIT_R2, 0, SLJIT_S2, 0, loop);
    OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_R1, 0);
    }
  else
    {
    GET_LOCAL_BASE(SLJIT_R0, 0, OVECTOR_START + (topbracket - 1) * 2 * sizeof(sljit_sw));
    OP1(SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, topbracket + 1);

    /* OVECTOR(0) is never equal to SLJIT_S2. */
    loop = LABEL();
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_R0), 0);
    OP2(SLJIT_SUB, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 2 * (sljit_sw)sizeof(sljit_sw));
    OP2(SLJIT_SUB, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
    CMPTO(SLJIT_EQUAL, SLJIT_R2, 0, SLJIT_S2, 0, loop);
    OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_R1, 0);
    }
  }
else
  OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, 1);
}