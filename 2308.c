static SLJIT_INLINE void compile_then_trap_matchingpath(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
BOOL needs_control_head;
int size;

PUSH_BACKTRACK_NOVALUE(sizeof(then_trap_backtrack), cc);
common->then_trap = BACKTRACK_AS(then_trap_backtrack);
BACKTRACK_AS(then_trap_backtrack)->common.cc = then_trap_opcode;
BACKTRACK_AS(then_trap_backtrack)->start = (sljit_sw)(cc - common->start);
BACKTRACK_AS(then_trap_backtrack)->framesize = get_framesize(common, cc, ccend, FALSE, &needs_control_head);

size = BACKTRACK_AS(then_trap_backtrack)->framesize;
size = 3 + (size < 0 ? 0 : size);

OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
allocate_stack(common, size);
if (size > 3)
  OP2(SLJIT_ADD, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, STACK_TOP, 0, SLJIT_IMM, (size - 3) * sizeof(sljit_sw));
else
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, STACK_TOP, 0);
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(size - 1), SLJIT_IMM, BACKTRACK_AS(then_trap_backtrack)->start);
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(size - 2), SLJIT_IMM, type_then_trap);
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(size - 3), TMP2, 0);

size = BACKTRACK_AS(then_trap_backtrack)->framesize;
if (size >= 0)
  init_frame(common, cc, ccend, size - 1, 0);
}