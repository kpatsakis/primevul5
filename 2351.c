static void compile_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
then_trap_backtrack *save_then_trap = common->then_trap;

while (current)
  {
  if (current->nextbacktracks != NULL)
    set_jumps(current->nextbacktracks, LABEL());
  switch(*current->cc)
    {
    case OP_SET_SOM:
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(0), TMP1, 0);
    break;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_UPTO:
    case OP_MINUPTO:
    case OP_EXACT:
    case OP_POSSTAR:
    case OP_POSPLUS:
    case OP_POSQUERY:
    case OP_POSUPTO:
    case OP_STARI:
    case OP_MINSTARI:
    case OP_PLUSI:
    case OP_MINPLUSI:
    case OP_QUERYI:
    case OP_MINQUERYI:
    case OP_UPTOI:
    case OP_MINUPTOI:
    case OP_EXACTI:
    case OP_POSSTARI:
    case OP_POSPLUSI:
    case OP_POSQUERYI:
    case OP_POSUPTOI:
    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    case OP_NOTEXACT:
    case OP_NOTPOSSTAR:
    case OP_NOTPOSPLUS:
    case OP_NOTPOSQUERY:
    case OP_NOTPOSUPTO:
    case OP_NOTSTARI:
    case OP_NOTMINSTARI:
    case OP_NOTPLUSI:
    case OP_NOTMINPLUSI:
    case OP_NOTQUERYI:
    case OP_NOTMINQUERYI:
    case OP_NOTUPTOI:
    case OP_NOTMINUPTOI:
    case OP_NOTEXACTI:
    case OP_NOTPOSSTARI:
    case OP_NOTPOSPLUSI:
    case OP_NOTPOSQUERYI:
    case OP_NOTPOSUPTOI:
    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    case OP_TYPEEXACT:
    case OP_TYPEPOSSTAR:
    case OP_TYPEPOSPLUS:
    case OP_TYPEPOSQUERY:
    case OP_TYPEPOSUPTO:
    case OP_CLASS:
    case OP_NCLASS:
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
    case OP_XCLASS:
#endif
    compile_iterator_backtrackingpath(common, current);
    break;

    case OP_REF:
    case OP_REFI:
    case OP_DNREF:
    case OP_DNREFI:
    compile_ref_iterator_backtrackingpath(common, current);
    break;

    case OP_RECURSE:
    compile_recurse_backtrackingpath(common, current);
    break;

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    compile_assert_backtrackingpath(common, current);
    break;

    case OP_ASSERT_NA:
    case OP_ASSERTBACK_NA:
    case OP_ONCE:
    case OP_SCRIPT_RUN:
    case OP_BRA:
    case OP_CBRA:
    case OP_COND:
    case OP_SBRA:
    case OP_SCBRA:
    case OP_SCOND:
    compile_bracket_backtrackingpath(common, current);
    break;

    case OP_BRAZERO:
    if (current->cc[1] > OP_ASSERTBACK_NOT)
      compile_bracket_backtrackingpath(common, current);
    else
      compile_assert_backtrackingpath(common, current);
    break;

    case OP_BRAPOS:
    case OP_CBRAPOS:
    case OP_SBRAPOS:
    case OP_SCBRAPOS:
    case OP_BRAPOSZERO:
    compile_bracketpos_backtrackingpath(common, current);
    break;

    case OP_BRAMINZERO:
    compile_braminzero_backtrackingpath(common, current);
    break;

    case OP_MARK:
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(common->has_skip_arg ? 4 : 0));
    if (common->has_skip_arg)
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, common->has_skip_arg ? 5 : 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->mark_ptr, TMP1, 0);
    if (common->has_skip_arg)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, TMP2, 0);
    break;

    case OP_THEN:
    case OP_THEN_ARG:
    case OP_PRUNE:
    case OP_PRUNE_ARG:
    case OP_SKIP:
    case OP_SKIP_ARG:
    compile_control_verb_backtrackingpath(common, current);
    break;

    case OP_COMMIT:
    case OP_COMMIT_ARG:
    if (!common->local_quit_available)
      OP1(SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, PCRE2_ERROR_NOMATCH);
    if (common->quit_label == NULL)
      add_jump(compiler, &common->quit, JUMP(SLJIT_JUMP));
    else
      JUMPTO(SLJIT_JUMP, common->quit_label);
    break;

    case OP_CALLOUT:
    case OP_CALLOUT_STR:
    case OP_FAIL:
    case OP_ACCEPT:
    case OP_ASSERT_ACCEPT:
    set_jumps(current->topbacktracks, LABEL());
    break;

    case OP_THEN_TRAP:
    /* A virtual opcode for then traps. */
    compile_then_trap_backtrackingpath(common, current);
    break;

    default:
    SLJIT_UNREACHABLE();
    break;
    }
  current = current->prev;
  }
common->then_trap = save_then_trap;
}