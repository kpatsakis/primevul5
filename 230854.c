static SLJIT_INLINE void do_reset_match(compiler_common *common, int length)
{
DEFINE_COMPILER;
struct sljit_label *loop;
int i;

SLJIT_ASSERT(length > 1);
/* OVECTOR(1) contains the "string begin - 1" constant. */
if (length > 2)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1));
if (length < 8)
  {
  for (i = 2; i < length; i++)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(i), TMP1, 0);
  }
else
  {
  if (sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_SUPP | SLJIT_MEM_STORE | SLJIT_MEM_PRE, TMP1, SLJIT_MEM1(TMP2), sizeof(sljit_sw)) == SLJIT_SUCCESS)
    {
    GET_LOCAL_BASE(TMP2, 0, OVECTOR_START + sizeof(sljit_sw));
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_IMM, length - 2);
    loop = LABEL();
    sljit_emit_mem(compiler, SLJIT_MOV | SLJIT_MEM_STORE | SLJIT_MEM_PRE, TMP1, SLJIT_MEM1(TMP2), sizeof(sljit_sw));
    OP2(SLJIT_SUB | SLJIT_SET_Z, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, loop);
    }
  else
    {
    GET_LOCAL_BASE(TMP2, 0, OVECTOR_START + 2 * sizeof(sljit_sw));
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_IMM, length - 2);
    loop = LABEL();
    OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), 0, TMP1, 0);
    OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, sizeof(sljit_sw));
    OP2(SLJIT_SUB | SLJIT_SET_Z, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, loop);
    }
  }

OP1(SLJIT_MOV, STACK_TOP, 0, ARGUMENTS, 0);
if (common->mark_ptr != 0)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->mark_ptr, SLJIT_IMM, 0);
if (common->control_head_ptr != 0)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_IMM, 0);
OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(STACK_TOP), SLJIT_OFFSETOF(jit_arguments, stack));
OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->start_ptr);
OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(STACK_TOP), SLJIT_OFFSETOF(struct sljit_stack, end));
}