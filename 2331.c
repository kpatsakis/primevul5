static void compile_assert_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
PCRE2_SPTR cc = current->cc;
PCRE2_UCHAR bra = OP_BRA;
struct sljit_jump *brajump = NULL;

SLJIT_ASSERT(*cc != OP_BRAMINZERO);
if (*cc == OP_BRAZERO)
  {
  bra = *cc;
  cc++;
  }

if (bra == OP_BRAZERO)
  {
  SLJIT_ASSERT(current->topbacktracks == NULL);
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  }

if (CURRENT_AS(assert_backtrack)->framesize < 0)
  {
  set_jumps(current->topbacktracks, LABEL());

  if (bra == OP_BRAZERO)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
    CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(assert_backtrack)->matchingpath);
    free_stack(common, 1);
    }
  return;
  }

if (bra == OP_BRAZERO)
  {
  if (*cc == OP_ASSERT_NOT || *cc == OP_ASSERTBACK_NOT)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
    CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(assert_backtrack)->matchingpath);
    free_stack(common, 1);
    return;
    }
  free_stack(common, 1);
  brajump = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
  }

if (*cc == OP_ASSERT || *cc == OP_ASSERTBACK)
  {
  OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), CURRENT_AS(assert_backtrack)->private_data_ptr);
  add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-2));
  OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (CURRENT_AS(assert_backtrack)->framesize - 1) * sizeof(sljit_sw));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), CURRENT_AS(assert_backtrack)->private_data_ptr, TMP1, 0);

  set_jumps(current->topbacktracks, LABEL());
  }
else
  set_jumps(current->topbacktracks, LABEL());

if (bra == OP_BRAZERO)
  {
  /* We know there is enough place on the stack. */
  OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
  JUMPTO(SLJIT_JUMP, CURRENT_AS(assert_backtrack)->matchingpath);
  JUMPHERE(brajump);
  }
}