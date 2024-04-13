static SLJIT_INLINE void compile_control_verb_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
PCRE2_UCHAR opcode = *current->cc;
struct sljit_label *loop;
struct sljit_jump *jump;

if (opcode == OP_THEN || opcode == OP_THEN_ARG)
  {
  if (common->then_trap != NULL)
    {
    SLJIT_ASSERT(common->control_head_ptr != 0);

    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, type_then_trap);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, common->then_trap->start);
    jump = JUMP(SLJIT_JUMP);

    loop = LABEL();
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    JUMPHERE(jump);
    CMPTO(SLJIT_NOT_EQUAL, SLJIT_MEM1(STACK_TOP), STACK(1), TMP1, 0, loop);
    CMPTO(SLJIT_NOT_EQUAL, SLJIT_MEM1(STACK_TOP), STACK(2), TMP2, 0, loop);
    add_jump(compiler, &common->then_trap->quit, JUMP(SLJIT_JUMP));
    return;
    }
  else if (!common->local_quit_available && common->in_positive_assertion)
    {
    add_jump(compiler, &common->positive_assertion_quit, JUMP(SLJIT_JUMP));
    return;
    }
  }

if (common->local_quit_available)
  {
  /* Abort match with a fail. */
  if (common->quit_label == NULL)
    add_jump(compiler, &common->quit, JUMP(SLJIT_JUMP));
  else
    JUMPTO(SLJIT_JUMP, common->quit_label);
  return;
  }

if (opcode == OP_SKIP_ARG)
  {
  SLJIT_ASSERT(common->control_head_ptr != 0 && TMP1 == SLJIT_R0 && STR_PTR == SLJIT_R1);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
  OP1(SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)(current->cc + 2));
  sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_RET(SW) | SLJIT_ARG1(SW) | SLJIT_ARG2(SW), SLJIT_IMM, SLJIT_FUNC_OFFSET(do_search_mark));

  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_R0, 0);
  add_jump(compiler, &common->reset_match, CMP(SLJIT_NOT_EQUAL, SLJIT_R0, 0, SLJIT_IMM, 0));
  return;
  }

if (opcode == OP_SKIP)
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
else
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_IMM, 0);
add_jump(compiler, &common->reset_match, JUMP(SLJIT_JUMP));
}