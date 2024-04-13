static void compile_bracket_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
int opcode, stacksize, alt_count, alt_max;
int offset = 0;
int private_data_ptr = CURRENT_AS(bracket_backtrack)->private_data_ptr;
int repeat_ptr = 0, repeat_type = 0, repeat_count = 0;
PCRE2_SPTR cc = current->cc;
PCRE2_SPTR ccbegin;
PCRE2_SPTR ccprev;
PCRE2_UCHAR bra = OP_BRA;
PCRE2_UCHAR ket;
assert_backtrack *assert;
BOOL has_alternatives;
BOOL needs_control_head = FALSE;
struct sljit_jump *brazero = NULL;
struct sljit_jump *next_alt = NULL;
struct sljit_jump *once = NULL;
struct sljit_jump *cond = NULL;
struct sljit_label *rmin_label = NULL;
struct sljit_label *exact_label = NULL;
struct sljit_put_label *put_label = NULL;

if (*cc == OP_BRAZERO || *cc == OP_BRAMINZERO)
  {
  bra = *cc;
  cc++;
  }

opcode = *cc;
ccbegin = bracketend(cc) - 1 - LINK_SIZE;
ket = *ccbegin;
if (ket == OP_KET && PRIVATE_DATA(ccbegin) != 0)
  {
  repeat_ptr = PRIVATE_DATA(ccbegin);
  repeat_type = PRIVATE_DATA(ccbegin + 2);
  repeat_count = PRIVATE_DATA(ccbegin + 3);
  SLJIT_ASSERT(repeat_type != 0 && repeat_count != 0);
  if (repeat_type == OP_UPTO)
    ket = OP_KETRMAX;
  if (repeat_type == OP_MINUPTO)
    ket = OP_KETRMIN;
  }
ccbegin = cc;
cc += GET(cc, 1);
has_alternatives = *cc == OP_ALT;
if (SLJIT_UNLIKELY(opcode == OP_COND) || SLJIT_UNLIKELY(opcode == OP_SCOND))
  has_alternatives = (ccbegin[1 + LINK_SIZE] >= OP_ASSERT && ccbegin[1 + LINK_SIZE] <= OP_ASSERTBACK_NOT) || CURRENT_AS(bracket_backtrack)->u.condfailed != NULL;
if (opcode == OP_CBRA || opcode == OP_SCBRA)
  offset = (GET2(ccbegin, 1 + LINK_SIZE)) << 1;
if (SLJIT_UNLIKELY(opcode == OP_COND) && (*cc == OP_KETRMAX || *cc == OP_KETRMIN))
  opcode = OP_SCOND;

alt_max = has_alternatives ? no_alternatives(ccbegin) : 0;

/* Decoding the needs_control_head in framesize. */
if (opcode == OP_ONCE)
  {
  needs_control_head = (CURRENT_AS(bracket_backtrack)->u.framesize & 0x1) != 0;
  CURRENT_AS(bracket_backtrack)->u.framesize >>= 1;
  }

if (ket != OP_KET && repeat_type != 0)
  {
  /* TMP1 is used in OP_KETRMIN below. */
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);
  if (repeat_type == OP_UPTO)
    OP2(SLJIT_ADD, SLJIT_MEM1(SLJIT_SP), repeat_ptr, TMP1, 0, SLJIT_IMM, 1);
  else
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), repeat_ptr, TMP1, 0);
  }

if (ket == OP_KETRMAX)
  {
  if (bra == OP_BRAZERO)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, 1);
    brazero = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0);
    }
  }
else if (ket == OP_KETRMIN)
  {
  if (bra != OP_BRAMINZERO)
    {
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    if (repeat_type != 0)
      {
      /* TMP1 was set a few lines above. */
      CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0, CURRENT_AS(bracket_backtrack)->recursive_matchingpath);
      /* Drop STR_PTR for non-greedy plus quantifier. */
      if (opcode != OP_ONCE)
        free_stack(common, 1);
      }
    else if (opcode >= OP_SBRA || opcode == OP_ONCE)
      {
      /* Checking zero-length iteration. */
      if (opcode != OP_ONCE || CURRENT_AS(bracket_backtrack)->u.framesize < 0)
        CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, CURRENT_AS(bracket_backtrack)->recursive_matchingpath);
      else
        {
        OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
        CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_MEM1(TMP1), STACK(-CURRENT_AS(bracket_backtrack)->u.framesize - 2), CURRENT_AS(bracket_backtrack)->recursive_matchingpath);
        }
      /* Drop STR_PTR for non-greedy plus quantifier. */
      if (opcode != OP_ONCE)
        free_stack(common, 1);
      }
    else
      JUMPTO(SLJIT_JUMP, CURRENT_AS(bracket_backtrack)->recursive_matchingpath);
    }
  rmin_label = LABEL();
  if (repeat_type != 0)
    OP2(SLJIT_ADD, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, 1);
  }
else if (bra == OP_BRAZERO)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);
  brazero = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0);
  }
else if (repeat_type == OP_EXACT)
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, 1);
  exact_label = LABEL();
  }

if (offset != 0)
  {
  if (common->capture_last_ptr != 0)
    {
    SLJIT_ASSERT(common->optimized_cbracket[offset >> 1] == 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr, TMP1, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(2));
    free_stack(common, 3);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP2, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), TMP1, 0);
    }
  else if (common->optimized_cbracket[offset >> 1] == 0)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    free_stack(common, 2);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), TMP2, 0);
    }
  }

if (SLJIT_UNLIKELY(opcode == OP_ONCE))
  {
  if (CURRENT_AS(bracket_backtrack)->u.framesize >= 0)
    {
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
    OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (CURRENT_AS(bracket_backtrack)->u.framesize - 1) * sizeof(sljit_sw));
    }
  once = JUMP(SLJIT_JUMP);
  }
else if (SLJIT_UNLIKELY(opcode == OP_COND) || SLJIT_UNLIKELY(opcode == OP_SCOND))
  {
  if (has_alternatives)
    {
    /* Always exactly one alternative. */
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, 1);

    alt_max = 2;
    next_alt = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0);
    }
  }
else if (has_alternatives)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);

  if (alt_max > 3)
    {
    sljit_emit_ijump(compiler, SLJIT_JUMP, TMP1, 0);

    SLJIT_ASSERT(CURRENT_AS(bracket_backtrack)->u.matching_put_label);
    sljit_set_put_label(CURRENT_AS(bracket_backtrack)->u.matching_put_label, LABEL());
    sljit_emit_op0(compiler, SLJIT_ENDBR);
    }
  else
    next_alt = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0);
  }

COMPILE_BACKTRACKINGPATH(current->top);
if (current->topbacktracks)
  set_jumps(current->topbacktracks, LABEL());

if (SLJIT_UNLIKELY(opcode == OP_COND) || SLJIT_UNLIKELY(opcode == OP_SCOND))
  {
  /* Conditional block always has at most one alternative. */
  if (ccbegin[1 + LINK_SIZE] >= OP_ASSERT && ccbegin[1 + LINK_SIZE] <= OP_ASSERTBACK_NOT)
    {
    SLJIT_ASSERT(has_alternatives);
    assert = CURRENT_AS(bracket_backtrack)->u.assert;
    if (assert->framesize >= 0 && (ccbegin[1 + LINK_SIZE] == OP_ASSERT || ccbegin[1 + LINK_SIZE] == OP_ASSERTBACK))
      {
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), assert->private_data_ptr);
      add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-2));
      OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (assert->framesize - 1) * sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), assert->private_data_ptr, TMP1, 0);
      }
    cond = JUMP(SLJIT_JUMP);
    set_jumps(CURRENT_AS(bracket_backtrack)->u.assert->condfailed, LABEL());
    }
  else if (CURRENT_AS(bracket_backtrack)->u.condfailed != NULL)
    {
    SLJIT_ASSERT(has_alternatives);
    cond = JUMP(SLJIT_JUMP);
    set_jumps(CURRENT_AS(bracket_backtrack)->u.condfailed, LABEL());
    }
  else
    SLJIT_ASSERT(!has_alternatives);
  }

if (has_alternatives)
  {
  alt_count = 1;
  do
    {
    current->top = NULL;
    current->topbacktracks = NULL;
    current->nextbacktracks = NULL;
    /* Conditional blocks always have an additional alternative, even if it is empty. */
    if (*cc == OP_ALT)
      {
      ccprev = cc + 1 + LINK_SIZE;
      cc += GET(cc, 1);
      if (opcode != OP_COND && opcode != OP_SCOND)
        {
        if (opcode != OP_ONCE)
          {
          if (private_data_ptr != 0)
            OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
          else
            OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
          }
        else
          OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(needs_control_head ? 1 : 0));
        }
      compile_matchingpath(common, ccprev, cc, current);
      if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
        return;

      if (opcode == OP_ASSERT_NA || opcode == OP_ASSERTBACK_NA)
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);

      if (opcode == OP_SCRIPT_RUN)
        match_script_run_common(common, private_data_ptr, current);
      }

    /* Instructions after the current alternative is successfully matched. */
    /* There is a similar code in compile_bracket_matchingpath. */
    if (opcode == OP_ONCE)
      match_once_common(common, ket, CURRENT_AS(bracket_backtrack)->u.framesize, private_data_ptr, has_alternatives, needs_control_head);

    stacksize = 0;
    if (repeat_type == OP_MINUPTO)
      {
      /* We need to preserve the counter. TMP2 will be used below. */
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), repeat_ptr);
      stacksize++;
      }
    if (ket != OP_KET || bra != OP_BRA)
      stacksize++;
    if (offset != 0)
      {
      if (common->capture_last_ptr != 0)
        stacksize++;
      if (common->optimized_cbracket[offset >> 1] == 0)
        stacksize += 2;
      }
    if (opcode != OP_ONCE)
      stacksize++;

    if (stacksize > 0)
      allocate_stack(common, stacksize);

    stacksize = 0;
    if (repeat_type == OP_MINUPTO)
      {
      /* TMP2 was set above. */
      OP2(SLJIT_SUB, SLJIT_MEM1(STACK_TOP), STACK(stacksize), TMP2, 0, SLJIT_IMM, 1);
      stacksize++;
      }

    if (ket != OP_KET || bra != OP_BRA)
      {
      if (ket != OP_KET)
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), STR_PTR, 0);
      else
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), SLJIT_IMM, 0);
      stacksize++;
      }

    if (offset != 0)
      stacksize = match_capture_common(common, stacksize, offset, private_data_ptr);

    if (opcode != OP_ONCE)
      {
      if (alt_max <= 3)
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), SLJIT_IMM, alt_count);
      else
        put_label = sljit_emit_put_label(compiler, SLJIT_MEM1(STACK_TOP), STACK(stacksize));
      }

    if (offset != 0 && ket == OP_KETRMAX && common->optimized_cbracket[offset >> 1] != 0)
      {
      /* If ket is not OP_KETRMAX, this code path is executed after the jump to alternative_matchingpath. */
      SLJIT_ASSERT(private_data_ptr == OVECTOR(offset + 0));
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
      }

    JUMPTO(SLJIT_JUMP, CURRENT_AS(bracket_backtrack)->alternative_matchingpath);

    if (opcode != OP_ONCE)
      {
      if (alt_max <= 3)
        {
        JUMPHERE(next_alt);
        alt_count++;
        if (alt_count < alt_max)
          {
          SLJIT_ASSERT(alt_count == 2 && alt_max == 3);
          next_alt = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 1);
          }
        }
      else
        {
        sljit_set_put_label(put_label, LABEL());
        sljit_emit_op0(compiler, SLJIT_ENDBR);
        }
      }

    COMPILE_BACKTRACKINGPATH(current->top);
    if (current->topbacktracks)
      set_jumps(current->topbacktracks, LABEL());
    SLJIT_ASSERT(!current->nextbacktracks);
    }
  while (*cc == OP_ALT);

  if (cond != NULL)
    {
    SLJIT_ASSERT(opcode == OP_COND || opcode == OP_SCOND);
    assert = CURRENT_AS(bracket_backtrack)->u.assert;
    if ((ccbegin[1 + LINK_SIZE] == OP_ASSERT_NOT || ccbegin[1 + LINK_SIZE] == OP_ASSERTBACK_NOT) && assert->framesize >= 0)
      {
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), assert->private_data_ptr);
      add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-2));
      OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (assert->framesize - 1) * sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), assert->private_data_ptr, TMP1, 0);
      }
    JUMPHERE(cond);
    }

  /* Free the STR_PTR. */
  if (private_data_ptr == 0)
    free_stack(common, 1);
  }

if (offset != 0)
  {
  /* Using both tmp register is better for instruction scheduling. */
  if (common->optimized_cbracket[offset >> 1] != 0)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    free_stack(common, 2);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), TMP2, 0);
    }
  else
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
    }
  }
else if (opcode == OP_ASSERT_NA || opcode == OP_ASSERTBACK_NA || opcode == OP_SCRIPT_RUN || opcode == OP_SBRA || opcode == OP_SCOND)
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);
  }
else if (opcode == OP_ONCE)
  {
  cc = ccbegin + GET(ccbegin, 1);
  stacksize = needs_control_head ? 1 : 0;

  if (CURRENT_AS(bracket_backtrack)->u.framesize >= 0)
    {
    /* Reset head and drop saved frame. */
    stacksize += CURRENT_AS(bracket_backtrack)->u.framesize + ((ket != OP_KET || *cc == OP_ALT) ? 2 : 1);
    }
  else if (ket == OP_KETRMAX || (*cc == OP_ALT && ket != OP_KETRMIN))
    {
    /* The STR_PTR must be released. */
    stacksize++;
    }

  if (stacksize > 0)
    free_stack(common, stacksize);

  JUMPHERE(once);
  /* Restore previous private_data_ptr */
  if (CURRENT_AS(bracket_backtrack)->u.framesize >= 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_MEM1(STACK_TOP), STACK(-CURRENT_AS(bracket_backtrack)->u.framesize - 1));
  else if (ket == OP_KETRMIN)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    /* See the comment below. */
    free_stack(common, 2);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
    }
  }

if (repeat_type == OP_EXACT)
  {
  OP2(SLJIT_ADD, TMP1, 0, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), repeat_ptr, TMP1, 0);
  CMPTO(SLJIT_LESS_EQUAL, TMP1, 0, SLJIT_IMM, repeat_count, exact_label);
  }
else if (ket == OP_KETRMAX)
  {
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  if (bra != OP_BRAZERO)
    free_stack(common, 1);

  CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(bracket_backtrack)->recursive_matchingpath);
  if (bra == OP_BRAZERO)
    {
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
    JUMPTO(SLJIT_JUMP, CURRENT_AS(bracket_backtrack)->zero_matchingpath);
    JUMPHERE(brazero);
    free_stack(common, 1);
    }
  }
else if (ket == OP_KETRMIN)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));

  /* OP_ONCE removes everything in case of a backtrack, so we don't
  need to explicitly release the STR_PTR. The extra release would
  affect badly the free_stack(2) above. */
  if (opcode != OP_ONCE)
    free_stack(common, 1);
  CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0, rmin_label);
  if (opcode == OP_ONCE)
    free_stack(common, bra == OP_BRAMINZERO ? 2 : 1);
  else if (bra == OP_BRAMINZERO)
    free_stack(common, 1);
  }
else if (bra == OP_BRAZERO)
  {
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  JUMPTO(SLJIT_JUMP, CURRENT_AS(bracket_backtrack)->zero_matchingpath);
  JUMPHERE(brazero);
  }
}