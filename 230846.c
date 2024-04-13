static void compile_matchingpath(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
BOOL has_then_trap = FALSE;
then_trap_backtrack *save_then_trap = NULL;

SLJIT_ASSERT(*ccend == OP_END || (*ccend >= OP_ALT && *ccend <= OP_KETRPOS));

if (common->has_then && common->then_offsets[cc - common->start] != 0)
  {
  SLJIT_ASSERT(*ccend != OP_END && common->control_head_ptr != 0);
  has_then_trap = TRUE;
  save_then_trap = common->then_trap;
  /* Tail item on backtrack. */
  compile_then_trap_matchingpath(common, cc, ccend, parent);
  }

while (cc < ccend)
  {
  switch(*cc)
    {
    case OP_SOD:
    case OP_SOM:
    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    case OP_EODN:
    case OP_EOD:
    case OP_DOLL:
    case OP_DOLLM:
    case OP_CIRC:
    case OP_CIRCM:
    case OP_REVERSE:
    cc = compile_simple_assertion_matchingpath(common, *cc, cc + 1, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks);
    break;

    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    case OP_ANYBYTE:
    case OP_NOTPROP:
    case OP_PROP:
    case OP_ANYNL:
    case OP_NOT_HSPACE:
    case OP_HSPACE:
    case OP_NOT_VSPACE:
    case OP_VSPACE:
    case OP_EXTUNI:
    case OP_NOT:
    case OP_NOTI:
    cc = compile_char1_matchingpath(common, *cc, cc + 1, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE);
    break;

    case OP_SET_SOM:
    PUSH_BACKTRACK_NOVALUE(sizeof(backtrack_common), cc);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(0));
    allocate_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(0), STR_PTR, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);
    cc++;
    break;

    case OP_CHAR:
    case OP_CHARI:
    if (common->mode == PCRE2_JIT_COMPLETE)
      cc = compile_charn_matchingpath(common, cc, ccend, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks);
    else
      cc = compile_char1_matchingpath(common, *cc, cc + 1, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE);
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
    cc = compile_iterator_matchingpath(common, cc, parent);
    break;

    case OP_CLASS:
    case OP_NCLASS:
    if (cc[1 + (32 / sizeof(PCRE2_UCHAR))] >= OP_CRSTAR && cc[1 + (32 / sizeof(PCRE2_UCHAR))] <= OP_CRPOSRANGE)
      cc = compile_iterator_matchingpath(common, cc, parent);
    else
      cc = compile_char1_matchingpath(common, *cc, cc + 1, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE);
    break;

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
    case OP_XCLASS:
    if (*(cc + GET(cc, 1)) >= OP_CRSTAR && *(cc + GET(cc, 1)) <= OP_CRPOSRANGE)
      cc = compile_iterator_matchingpath(common, cc, parent);
    else
      cc = compile_char1_matchingpath(common, *cc, cc + 1, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE);
    break;
#endif

    case OP_REF:
    case OP_REFI:
    if (cc[1 + IMM2_SIZE] >= OP_CRSTAR && cc[1 + IMM2_SIZE] <= OP_CRPOSRANGE)
      cc = compile_ref_iterator_matchingpath(common, cc, parent);
    else
      {
      compile_ref_matchingpath(common, cc, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE, FALSE);
      cc += 1 + IMM2_SIZE;
      }
    break;

    case OP_DNREF:
    case OP_DNREFI:
    if (cc[1 + 2 * IMM2_SIZE] >= OP_CRSTAR && cc[1 + 2 * IMM2_SIZE] <= OP_CRPOSRANGE)
      cc = compile_ref_iterator_matchingpath(common, cc, parent);
    else
      {
      compile_dnref_search(common, cc, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks);
      compile_ref_matchingpath(common, cc, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, TRUE, FALSE);
      cc += 1 + 2 * IMM2_SIZE;
      }
    break;

    case OP_RECURSE:
    cc = compile_recurse_matchingpath(common, cc, parent);
    break;

    case OP_CALLOUT:
    case OP_CALLOUT_STR:
    cc = compile_callout_matchingpath(common, cc, parent);
    break;

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    PUSH_BACKTRACK_NOVALUE(sizeof(assert_backtrack), cc);
    cc = compile_assert_matchingpath(common, cc, BACKTRACK_AS(assert_backtrack), FALSE);
    break;

    case OP_BRAMINZERO:
    PUSH_BACKTRACK_NOVALUE(sizeof(braminzero_backtrack), cc);
    cc = bracketend(cc + 1);
    if (*(cc - 1 - LINK_SIZE) != OP_KETRMIN)
      {
      allocate_stack(common, 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
      }
    else
      {
      allocate_stack(common, 2);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), STR_PTR, 0);
      }
    BACKTRACK_AS(braminzero_backtrack)->matchingpath = LABEL();
    count_match(common);
    break;

    case OP_ONCE:
    case OP_BRA:
    case OP_CBRA:
    case OP_COND:
    case OP_SBRA:
    case OP_SCBRA:
    case OP_SCOND:
    cc = compile_bracket_matchingpath(common, cc, parent);
    break;

    case OP_BRAZERO:
    if (cc[1] > OP_ASSERTBACK_NOT)
      cc = compile_bracket_matchingpath(common, cc, parent);
    else
      {
      PUSH_BACKTRACK_NOVALUE(sizeof(assert_backtrack), cc);
      cc = compile_assert_matchingpath(common, cc, BACKTRACK_AS(assert_backtrack), FALSE);
      }
    break;

    case OP_BRAPOS:
    case OP_CBRAPOS:
    case OP_SBRAPOS:
    case OP_SCBRAPOS:
    case OP_BRAPOSZERO:
    cc = compile_bracketpos_matchingpath(common, cc, parent);
    break;

    case OP_MARK:
    PUSH_BACKTRACK_NOVALUE(sizeof(backtrack_common), cc);
    SLJIT_ASSERT(common->mark_ptr != 0);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->mark_ptr);
    allocate_stack(common, common->has_skip_arg ? 5 : 1);
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(common->has_skip_arg ? 4 : 0), TMP2, 0);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, (sljit_sw)(cc + 2));
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->mark_ptr, TMP2, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, mark_ptr), TMP2, 0);
    if (common->has_skip_arg)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, STACK_TOP, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, type_mark);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(2), SLJIT_IMM, (sljit_sw)(cc + 2));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(3), STR_PTR, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP1, 0);
      }
    cc += 1 + 2 + cc[1];
    break;

    case OP_PRUNE:
    case OP_PRUNE_ARG:
    case OP_SKIP:
    case OP_SKIP_ARG:
    case OP_THEN:
    case OP_THEN_ARG:
    case OP_COMMIT:
    case OP_COMMIT_ARG:
    cc = compile_control_verb_matchingpath(common, cc, parent);
    break;

    case OP_FAIL:
    case OP_ACCEPT:
    case OP_ASSERT_ACCEPT:
    cc = compile_fail_accept_matchingpath(common, cc, parent);
    break;

    case OP_CLOSE:
    cc = compile_close_matchingpath(common, cc);
    break;

    case OP_SKIPZERO:
    cc = bracketend(cc + 1);
    break;

    default:
    SLJIT_UNREACHABLE();
    return;
    }
  if (cc == NULL)
    return;
  }

if (has_then_trap)
  {
  /* Head item on backtrack. */
  PUSH_BACKTRACK_NOVALUE(sizeof(then_trap_backtrack), cc);
  BACKTRACK_AS(then_trap_backtrack)->common.cc = then_trap_opcode;
  BACKTRACK_AS(then_trap_backtrack)->then_trap = common->then_trap;
  common->then_trap = save_then_trap;
  }
SLJIT_ASSERT(cc == ccend);
}