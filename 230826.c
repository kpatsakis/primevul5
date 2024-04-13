static PCRE2_SPTR compile_bracket_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
PCRE2_UCHAR opcode;
int private_data_ptr = 0;
int offset = 0;
int i, stacksize;
int repeat_ptr = 0, repeat_length = 0;
int repeat_type = 0, repeat_count = 0;
PCRE2_SPTR ccbegin;
PCRE2_SPTR matchingpath;
PCRE2_SPTR slot;
PCRE2_UCHAR bra = OP_BRA;
PCRE2_UCHAR ket;
assert_backtrack *assert;
BOOL has_alternatives;
BOOL needs_control_head = FALSE;
struct sljit_jump *jump;
struct sljit_jump *skip;
struct sljit_label *rmax_label = NULL;
struct sljit_jump *braminzero = NULL;

PUSH_BACKTRACK(sizeof(bracket_backtrack), cc, NULL);

if (*cc == OP_BRAZERO || *cc == OP_BRAMINZERO)
  {
  bra = *cc;
  cc++;
  opcode = *cc;
  }

opcode = *cc;
ccbegin = cc;
matchingpath = bracketend(cc) - 1 - LINK_SIZE;
ket = *matchingpath;
if (ket == OP_KET && PRIVATE_DATA(matchingpath) != 0)
  {
  repeat_ptr = PRIVATE_DATA(matchingpath);
  repeat_length = PRIVATE_DATA(matchingpath + 1);
  repeat_type = PRIVATE_DATA(matchingpath + 2);
  repeat_count = PRIVATE_DATA(matchingpath + 3);
  SLJIT_ASSERT(repeat_length != 0 && repeat_type != 0 && repeat_count != 0);
  if (repeat_type == OP_UPTO)
    ket = OP_KETRMAX;
  if (repeat_type == OP_MINUPTO)
    ket = OP_KETRMIN;
  }

matchingpath = ccbegin + 1 + LINK_SIZE;
SLJIT_ASSERT(ket == OP_KET || ket == OP_KETRMAX || ket == OP_KETRMIN);
SLJIT_ASSERT(!((bra == OP_BRAZERO && ket == OP_KETRMIN) || (bra == OP_BRAMINZERO && ket == OP_KETRMAX)));
cc += GET(cc, 1);

has_alternatives = *cc == OP_ALT;
if (SLJIT_UNLIKELY(opcode == OP_COND || opcode == OP_SCOND))
  {
  SLJIT_COMPILE_ASSERT(OP_DNRREF == OP_RREF + 1 && OP_FALSE == OP_RREF + 2 && OP_TRUE == OP_RREF + 3,
    compile_time_checks_must_be_grouped_together);
  has_alternatives = ((*matchingpath >= OP_RREF && *matchingpath <= OP_TRUE) || *matchingpath == OP_FAIL) ? FALSE : TRUE;
  }

if (SLJIT_UNLIKELY(opcode == OP_COND) && (*cc == OP_KETRMAX || *cc == OP_KETRMIN))
  opcode = OP_SCOND;

if (opcode == OP_CBRA || opcode == OP_SCBRA)
  {
  /* Capturing brackets has a pre-allocated space. */
  offset = GET2(ccbegin, 1 + LINK_SIZE);
  if (common->optimized_cbracket[offset] == 0)
    {
    private_data_ptr = OVECTOR_PRIV(offset);
    offset <<= 1;
    }
  else
    {
    offset <<= 1;
    private_data_ptr = OVECTOR(offset);
    }
  BACKTRACK_AS(bracket_backtrack)->private_data_ptr = private_data_ptr;
  matchingpath += IMM2_SIZE;
  }
else if (opcode == OP_ONCE || opcode == OP_SBRA || opcode == OP_SCOND)
  {
  /* Other brackets simply allocate the next entry. */
  private_data_ptr = PRIVATE_DATA(ccbegin);
  SLJIT_ASSERT(private_data_ptr != 0);
  BACKTRACK_AS(bracket_backtrack)->private_data_ptr = private_data_ptr;
  if (opcode == OP_ONCE)
    BACKTRACK_AS(bracket_backtrack)->u.framesize = get_framesize(common, ccbegin, NULL, FALSE, &needs_control_head);
  }

/* Instructions before the first alternative. */
stacksize = 0;
if (ket == OP_KETRMAX || (ket == OP_KETRMIN && bra != OP_BRAMINZERO))
  stacksize++;
if (bra == OP_BRAZERO)
  stacksize++;

if (stacksize > 0)
  allocate_stack(common, stacksize);

stacksize = 0;
if (ket == OP_KETRMAX || (ket == OP_KETRMIN && bra != OP_BRAMINZERO))
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), SLJIT_IMM, 0);
  stacksize++;
  }

if (bra == OP_BRAZERO)
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), STR_PTR, 0);

if (bra == OP_BRAMINZERO)
  {
  /* This is a backtrack path! (Since the try-path of OP_BRAMINZERO matches to the empty string) */
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  if (ket != OP_KETRMIN)
    {
    free_stack(common, 1);
    braminzero = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
    }
  else
    {
    if (opcode == OP_ONCE || opcode >= OP_SBRA)
      {
      jump = CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
      /* Nothing stored during the first run. */
      skip = JUMP(SLJIT_JUMP);
      JUMPHERE(jump);
      /* Checking zero-length iteration. */
      if (opcode != OP_ONCE || BACKTRACK_AS(bracket_backtrack)->u.framesize < 0)
        {
        /* When we come from outside, private_data_ptr contains the previous STR_PTR. */
        braminzero = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
        }
      else
        {
        /* Except when the whole stack frame must be saved. */
        OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
        braminzero = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_MEM1(TMP1), STACK(-BACKTRACK_AS(bracket_backtrack)->u.framesize - 2));
        }
      JUMPHERE(skip);
      }
    else
      {
      jump = CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
      JUMPHERE(jump);
      }
    }
  }

if (repeat_type != 0)
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, repeat_count);
  if (repeat_type == OP_EXACT)
    rmax_label = LABEL();
  }

if (ket == OP_KETRMIN)
  BACKTRACK_AS(bracket_backtrack)->recursive_matchingpath = LABEL();

if (ket == OP_KETRMAX)
  {
  rmax_label = LABEL();
  if (has_alternatives && opcode != OP_ONCE && opcode < OP_SBRA && repeat_type == 0)
    BACKTRACK_AS(bracket_backtrack)->alternative_matchingpath = rmax_label;
  }

/* Handling capturing brackets and alternatives. */
if (opcode == OP_ONCE)
  {
  stacksize = 0;
  if (needs_control_head)
    {
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
    stacksize++;
    }

  if (BACKTRACK_AS(bracket_backtrack)->u.framesize < 0)
    {
    /* Neither capturing brackets nor recursions are found in the block. */
    if (ket == OP_KETRMIN)
      {
      stacksize += 2;
      if (!needs_control_head)
        OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      }
    else
      {
      if (BACKTRACK_AS(bracket_backtrack)->u.framesize == no_frame)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STACK_TOP, 0);
      if (ket == OP_KETRMAX || has_alternatives)
        stacksize++;
      }

    if (stacksize > 0)
      allocate_stack(common, stacksize);

    stacksize = 0;
    if (needs_control_head)
      {
      stacksize++;
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);
      }

    if (ket == OP_KETRMIN)
      {
      if (needs_control_head)
        OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), STR_PTR, 0);
      if (BACKTRACK_AS(bracket_backtrack)->u.framesize == no_frame)
        OP2(SLJIT_ADD, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STACK_TOP, 0, SLJIT_IMM, needs_control_head ? (2 * sizeof(sljit_sw)) : sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize + 1), TMP2, 0);
      }
    else if (ket == OP_KETRMAX || has_alternatives)
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), STR_PTR, 0);
    }
  else
    {
    if (ket != OP_KET || has_alternatives)
      stacksize++;

    stacksize += BACKTRACK_AS(bracket_backtrack)->u.framesize + 1;
    allocate_stack(common, stacksize);

    if (needs_control_head)
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);

    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    OP2(SLJIT_ADD, TMP2, 0, STACK_TOP, 0, SLJIT_IMM, stacksize * sizeof(sljit_sw));

    stacksize = needs_control_head ? 1 : 0;
    if (ket != OP_KET || has_alternatives)
      {
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), STR_PTR, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP2, 0);
      stacksize++;
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), TMP1, 0);
      }
    else
      {
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP2, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), TMP1, 0);
      }
    init_frame(common, ccbegin, NULL, BACKTRACK_AS(bracket_backtrack)->u.framesize + stacksize, stacksize + 1);
    }
  }
else if (opcode == OP_CBRA || opcode == OP_SCBRA)
  {
  /* Saving the previous values. */
  if (common->optimized_cbracket[offset >> 1] != 0)
    {
    SLJIT_ASSERT(private_data_ptr == OVECTOR(offset));
    allocate_stack(common, 2);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr + sizeof(sljit_sw));
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STR_PTR, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP1, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP2, 0);
    }
  else
    {
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    allocate_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STR_PTR, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);
    }
  }
else if (opcode == OP_SBRA || opcode == OP_SCOND)
  {
  /* Saving the previous value. */
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
  allocate_stack(common, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STR_PTR, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);
  }
else if (has_alternatives)
  {
  /* Pushing the starting string pointer. */
  allocate_stack(common, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
  }

/* Generating code for the first alternative. */
if (opcode == OP_COND || opcode == OP_SCOND)
  {
  if (*matchingpath == OP_CREF)
    {
    SLJIT_ASSERT(has_alternatives);
    add_jump(compiler, &(BACKTRACK_AS(bracket_backtrack)->u.condfailed),
      CMP(SLJIT_EQUAL, SLJIT_MEM1(SLJIT_SP), OVECTOR(GET2(matchingpath, 1) << 1), SLJIT_MEM1(SLJIT_SP), OVECTOR(1)));
    matchingpath += 1 + IMM2_SIZE;
    }
  else if (*matchingpath == OP_DNCREF)
    {
    SLJIT_ASSERT(has_alternatives);

    i = GET2(matchingpath, 1 + IMM2_SIZE);
    slot = common->name_table + GET2(matchingpath, 1) * common->name_entry_size;
    OP1(SLJIT_MOV, TMP3, 0, STR_PTR, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1));
    OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(GET2(slot, 0) << 1), TMP1, 0);
    slot += common->name_entry_size;
    i--;
    while (i-- > 0)
      {
      OP2(SLJIT_SUB, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(GET2(slot, 0) << 1), TMP1, 0);
      OP2(SLJIT_OR | SLJIT_SET_Z, TMP2, 0, TMP2, 0, STR_PTR, 0);
      slot += common->name_entry_size;
      }
    OP1(SLJIT_MOV, STR_PTR, 0, TMP3, 0);
    add_jump(compiler, &(BACKTRACK_AS(bracket_backtrack)->u.condfailed), JUMP(SLJIT_ZERO));
    matchingpath += 1 + 2 * IMM2_SIZE;
    }
  else if ((*matchingpath >= OP_RREF && *matchingpath <= OP_TRUE) || *matchingpath == OP_FAIL)
    {
    /* Never has other case. */
    BACKTRACK_AS(bracket_backtrack)->u.condfailed = NULL;
    SLJIT_ASSERT(!has_alternatives);

    if (*matchingpath == OP_TRUE)
      {
      stacksize = 1;
      matchingpath++;
      }
    else if (*matchingpath == OP_FALSE || *matchingpath == OP_FAIL)
      stacksize = 0;
    else if (*matchingpath == OP_RREF)
      {
      stacksize = GET2(matchingpath, 1);
      if (common->currententry == NULL)
        stacksize = 0;
      else if (stacksize == RREF_ANY)
        stacksize = 1;
      else if (common->currententry->start == 0)
        stacksize = stacksize == 0;
      else
        stacksize = stacksize == (int)GET2(common->start, common->currententry->start + 1 + LINK_SIZE);

      if (stacksize != 0)
        matchingpath += 1 + IMM2_SIZE;
      }
    else
      {
      if (common->currententry == NULL || common->currententry->start == 0)
        stacksize = 0;
      else
        {
        stacksize = GET2(matchingpath, 1 + IMM2_SIZE);
        slot = common->name_table + GET2(matchingpath, 1) * common->name_entry_size;
        i = (int)GET2(common->start, common->currententry->start + 1 + LINK_SIZE);
        while (stacksize > 0)
          {
          if ((int)GET2(slot, 0) == i)
            break;
          slot += common->name_entry_size;
          stacksize--;
          }
        }

      if (stacksize != 0)
        matchingpath += 1 + 2 * IMM2_SIZE;
      }

      /* The stacksize == 0 is a common "else" case. */
      if (stacksize == 0)
        {
        if (*cc == OP_ALT)
          {
          matchingpath = cc + 1 + LINK_SIZE;
          cc += GET(cc, 1);
          }
        else
          matchingpath = cc;
        }
    }
  else
    {
    SLJIT_ASSERT(has_alternatives && *matchingpath >= OP_ASSERT && *matchingpath <= OP_ASSERTBACK_NOT);
    /* Similar code as PUSH_BACKTRACK macro. */
    assert = sljit_alloc_memory(compiler, sizeof(assert_backtrack));
    if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
      return NULL;
    memset(assert, 0, sizeof(assert_backtrack));
    assert->common.cc = matchingpath;
    BACKTRACK_AS(bracket_backtrack)->u.assert = assert;
    matchingpath = compile_assert_matchingpath(common, matchingpath, assert, TRUE);
    }
  }

compile_matchingpath(common, matchingpath, cc, backtrack);
if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
  return NULL;

if (opcode == OP_ONCE)
  match_once_common(common, ket, BACKTRACK_AS(bracket_backtrack)->u.framesize, private_data_ptr, has_alternatives, needs_control_head);

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
if (has_alternatives && opcode != OP_ONCE)
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

if (has_alternatives)
  {
  if (opcode != OP_ONCE)
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), SLJIT_IMM, 0);
  if (ket != OP_KETRMAX)
    BACKTRACK_AS(bracket_backtrack)->alternative_matchingpath = LABEL();
  }

/* Must be after the matchingpath label. */
if (offset != 0 && common->optimized_cbracket[offset >> 1] != 0)
  {
  SLJIT_ASSERT(private_data_ptr == OVECTOR(offset + 0));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
  }

if (ket == OP_KETRMAX)
  {
  if (repeat_type != 0)
    {
    if (has_alternatives)
      BACKTRACK_AS(bracket_backtrack)->alternative_matchingpath = LABEL();
    OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, rmax_label);
    /* Drop STR_PTR for greedy plus quantifier. */
    if (opcode != OP_ONCE)
      free_stack(common, 1);
    }
  else if (opcode == OP_ONCE || opcode >= OP_SBRA)
    {
    if (has_alternatives)
      BACKTRACK_AS(bracket_backtrack)->alternative_matchingpath = LABEL();
    /* Checking zero-length iteration. */
    if (opcode != OP_ONCE)
      {
      CMPTO(SLJIT_NOT_EQUAL, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STR_PTR, 0, rmax_label);
      /* Drop STR_PTR for greedy plus quantifier. */
      if (bra != OP_BRAZERO)
        free_stack(common, 1);
      }
    else
      /* TMP2 must contain the starting STR_PTR. */
      CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, STR_PTR, 0, rmax_label);
    }
  else
    JUMPTO(SLJIT_JUMP, rmax_label);
  BACKTRACK_AS(bracket_backtrack)->recursive_matchingpath = LABEL();
  }

if (repeat_type == OP_EXACT)
  {
  count_match(common);
  OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_MEM1(SLJIT_SP), repeat_ptr, SLJIT_IMM, 1);
  JUMPTO(SLJIT_NOT_ZERO, rmax_label);
  }
else if (repeat_type == OP_UPTO)
  {
  /* We need to preserve the counter. */
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), repeat_ptr);
  allocate_stack(common, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);
  }

if (bra == OP_BRAZERO)
  BACKTRACK_AS(bracket_backtrack)->zero_matchingpath = LABEL();

if (bra == OP_BRAMINZERO)
  {
  /* This is a backtrack path! (From the viewpoint of OP_BRAMINZERO) */
  JUMPTO(SLJIT_JUMP, ((braminzero_backtrack *)parent)->matchingpath);
  if (braminzero != NULL)
    {
    JUMPHERE(braminzero);
    /* We need to release the end pointer to perform the
    backtrack for the zero-length iteration. When
    framesize is < 0, OP_ONCE will do the release itself. */
    if (opcode == OP_ONCE && BACKTRACK_AS(bracket_backtrack)->u.framesize >= 0)
      {
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
      OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (BACKTRACK_AS(bracket_backtrack)->u.framesize - 1) * sizeof(sljit_sw));
      }
    else if (ket == OP_KETRMIN && opcode != OP_ONCE)
      free_stack(common, 1);
    }
  /* Continue to the normal backtrack. */
  }

if ((ket != OP_KET && bra != OP_BRAMINZERO) || bra == OP_BRAZERO)
  count_match(common);

/* Skip the other alternatives. */
while (*cc == OP_ALT)
  cc += GET(cc, 1);
cc += 1 + LINK_SIZE;

if (opcode == OP_ONCE)
  {
  /* We temporarily encode the needs_control_head in the lowest bit.
     Note: on the target architectures of SLJIT the ((x << 1) >> 1) returns
     the same value for small signed numbers (including negative numbers). */
  BACKTRACK_AS(bracket_backtrack)->u.framesize = (BACKTRACK_AS(bracket_backtrack)->u.framesize << 1) | (needs_control_head ? 1 : 0);
  }
return cc + repeat_length;
}