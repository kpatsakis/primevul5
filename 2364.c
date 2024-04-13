static SLJIT_INLINE void compile_then_trap_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
struct sljit_jump *jump;
int size;

if (CURRENT_AS(then_trap_backtrack)->then_trap)
  {
  common->then_trap = CURRENT_AS(then_trap_backtrack)->then_trap;
  return;
  }

size = CURRENT_AS(then_trap_backtrack)->framesize;
size = 3 + (size < 0 ? 0 : size);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(size - 3));
free_stack(common, size);
jump = JUMP(SLJIT_JUMP);

set_jumps(CURRENT_AS(then_trap_backtrack)->quit, LABEL());
/* STACK_TOP is set by THEN. */
if (CURRENT_AS(then_trap_backtrack)->framesize >= 0)
  {
  add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
  OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (CURRENT_AS(then_trap_backtrack)->framesize - 1) * sizeof(sljit_sw));
  }
OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
free_stack(common, 3);

JUMPHERE(jump);
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, TMP1, 0);
}