static SLJIT_INLINE void match_once_common(compiler_common *common, PCRE2_UCHAR ket, int framesize, int private_data_ptr, BOOL has_alternatives, BOOL needs_control_head)
{
DEFINE_COMPILER;
int stacksize;

if (framesize < 0)
  {
  if (framesize == no_frame)
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
  else
    {
    stacksize = needs_control_head ? 1 : 0;
    if (ket != OP_KET || has_alternatives)
      stacksize++;

    if (stacksize > 0)
      free_stack(common, stacksize);
    }

  if (needs_control_head)
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), (ket != OP_KET || has_alternatives) ? STACK(-2) : STACK(-1));

  /* TMP2 which is set here used by OP_KETRMAX below. */
  if (ket == OP_KETRMAX)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(-1));
  else if (ket == OP_KETRMIN)
    {
    /* Move the STR_PTR to the private_data_ptr. */
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_MEM1(STACK_TOP), STACK(-1));
    }
  }
else
  {
  stacksize = (ket != OP_KET || has_alternatives) ? 2 : 1;
  OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, (framesize + stacksize) * sizeof(sljit_sw));
  if (needs_control_head)
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-1));

  if (ket == OP_KETRMAX)
    {
    /* TMP2 which is set here used by OP_KETRMAX below. */
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    }
  }
if (needs_control_head)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, TMP1, 0);
}