static SLJIT_INLINE PCRE2_SPTR compile_control_verb_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
PCRE2_UCHAR opcode = *cc;
PCRE2_SPTR ccend = cc + 1;

if (opcode == OP_COMMIT_ARG || opcode == OP_PRUNE_ARG ||
    opcode == OP_SKIP_ARG || opcode == OP_THEN_ARG)
  ccend += 2 + cc[1];

PUSH_BACKTRACK(sizeof(backtrack_common), cc, NULL);

if (opcode == OP_SKIP)
  {
  allocate_stack(common, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
  return ccend;
  }

if (opcode == OP_COMMIT_ARG || opcode == OP_PRUNE_ARG || opcode == OP_THEN_ARG)
  {
  OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, (sljit_sw)(cc + 2));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->mark_ptr, TMP2, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, mark_ptr), TMP2, 0);
  }

return ccend;
}