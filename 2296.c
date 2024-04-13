static SLJIT_INLINE void reset_ovector(compiler_common *common, int length)
{
DEFINE_COMPILER;
struct sljit_label *loop;
sljit_s32 i;

/* At this point we can freely use all temporary registers. */
SLJIT_ASSERT(length > 1);
/* TMP1 returns with begin - 1. */
OP2(SLJIT_SUB, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(jit_arguments, begin), SLJIT_IMM, IN_UCHARS(1));
if (length < 8)
  {
  for (i = 1; i < length; i++)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(i), SLJIT_R0, 0);
  }
else
  {
  if (sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_SUPP | SLJIT_MEM_STORE | SLJIT_MEM_PRE, SLJIT_R0, SLJIT_MEM1(SLJIT_R1), sizeof(sljit_sw)) == SLJIT_SUCCESS)
    {
    GET_LOCAL_BASE(SLJIT_R1, 0, OVECTOR_START);
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, length - 1);
    loop = LABEL();
    sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_STORE | SLJIT_MEM_PRE, SLJIT_R0, SLJIT_MEM1(SLJIT_R1), sizeof(sljit_sw));
    OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, loop);
    }
  else
    {
    GET_LOCAL_BASE(SLJIT_R1, 0, OVECTOR_START + sizeof(sljit_sw));
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, length - 1);
    loop = LABEL();
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_R1), 0, SLJIT_R0, 0);
    OP2(SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, sizeof(sljit_sw));
    OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, loop);
    }
  }
}