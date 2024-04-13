static SLJIT_INLINE void compile_ref_iterator_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
PCRE2_SPTR cc = current->cc;
BOOL ref = (*cc == OP_REF || *cc == OP_REFI);
PCRE2_UCHAR type;

type = cc[ref ? 1 + IMM2_SIZE : 1 + 2 * IMM2_SIZE];

if ((type & 0x1) == 0)
  {
  /* Maximize case. */
  set_jumps(current->topbacktracks, LABEL());
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);
  CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(ref_iterator_backtrack)->matchingpath);
  return;
  }

OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(ref_iterator_backtrack)->matchingpath);
set_jumps(current->topbacktracks, LABEL());
free_stack(common, ref ? 2 : 3);
}