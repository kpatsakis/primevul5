static SLJIT_INLINE void compile_bracketpos_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
int offset;
struct sljit_jump *jump;

if (CURRENT_AS(bracketpos_backtrack)->framesize < 0)
  {
  if (*current->cc == OP_CBRAPOS || *current->cc == OP_SCBRAPOS)
    {
    offset = (GET2(current->cc, 1 + LINK_SIZE)) << 1;
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
    if (common->capture_last_ptr != 0)
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(2));
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), TMP2, 0);
    if (common->capture_last_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr, TMP1, 0);
    }
  set_jumps(current->topbacktracks, LABEL());
  free_stack(common, CURRENT_AS(bracketpos_backtrack)->stacksize);
  return;
  }

OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), CURRENT_AS(bracketpos_backtrack)->private_data_ptr);
add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (CURRENT_AS(bracketpos_backtrack)->framesize - 1) * sizeof(sljit_sw));

if (current->topbacktracks)
  {
  jump = JUMP(SLJIT_JUMP);
  set_jumps(current->topbacktracks, LABEL());
  /* Drop the stack frame. */
  free_stack(common, CURRENT_AS(bracketpos_backtrack)->stacksize);
  JUMPHERE(jump);
  }
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), CURRENT_AS(bracketpos_backtrack)->private_data_ptr, SLJIT_MEM1(STACK_TOP), STACK(-CURRENT_AS(bracketpos_backtrack)->framesize - 1));
}