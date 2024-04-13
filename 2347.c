static PCRE2_SPTR compile_assert_matchingpath(compiler_common *common, PCRE2_SPTR cc, assert_backtrack *backtrack, BOOL conditional)
{
DEFINE_COMPILER;
int framesize;
int extrasize;
BOOL local_quit_available = FALSE;
BOOL needs_control_head;
int private_data_ptr;
backtrack_common altbacktrack;
PCRE2_SPTR ccbegin;
PCRE2_UCHAR opcode;
PCRE2_UCHAR bra = OP_BRA;
jump_list *tmp = NULL;
jump_list **target = (conditional) ? &backtrack->condfailed : &backtrack->common.topbacktracks;
jump_list **found;
/* Saving previous accept variables. */
BOOL save_local_quit_available = common->local_quit_available;
BOOL save_in_positive_assertion = common->in_positive_assertion;
then_trap_backtrack *save_then_trap = common->then_trap;
struct sljit_label *save_quit_label = common->quit_label;
struct sljit_label *save_accept_label = common->accept_label;
jump_list *save_quit = common->quit;
jump_list *save_positive_assertion_quit = common->positive_assertion_quit;
jump_list *save_accept = common->accept;
struct sljit_jump *jump;
struct sljit_jump *brajump = NULL;

/* Assert captures then. */
common->then_trap = NULL;

if (*cc == OP_BRAZERO || *cc == OP_BRAMINZERO)
  {
  SLJIT_ASSERT(!conditional);
  bra = *cc;
  cc++;
  }
private_data_ptr = PRIVATE_DATA(cc);
SLJIT_ASSERT(private_data_ptr != 0);
framesize = get_framesize(common, cc, NULL, FALSE, &needs_control_head);
backtrack->framesize = framesize;
backtrack->private_data_ptr = private_data_ptr;
opcode = *cc;
SLJIT_ASSERT(opcode >= OP_ASSERT && opcode <= OP_ASSERTBACK_NOT);
found = (opcode == OP_ASSERT || opcode == OP_ASSERTBACK) ? &tmp : target;
ccbegin = cc;
cc += GET(cc, 1);

if (bra == OP_BRAMINZERO)
  {
  /* This is a braminzero backtrack path. */
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
  free_stack(common, 1);
  brajump = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
  }

if (framesize < 0)
  {
  extrasize = 1;
  if (bra == OP_BRA && !assert_needs_str_ptr_saving(ccbegin + 1 + LINK_SIZE))
    extrasize = 0;

  if (needs_control_head)
    extrasize++;

  if (framesize == no_frame)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STACK_TOP, 0);

  if (extrasize > 0)
    allocate_stack(common, extrasize);

  if (needs_control_head)
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);

  if (extrasize > 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);

  if (needs_control_head)
    {
    SLJIT_ASSERT(extrasize == 2);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_IMM, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP1, 0);
    }
  }
else
  {
  extrasize = needs_control_head ? 3 : 2;
  allocate_stack(common, framesize + extrasize);

  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
  OP2(SLJIT_ADD, TMP2, 0, STACK_TOP, 0, SLJIT_IMM, (framesize + extrasize) * sizeof(sljit_sw));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP2, 0);
  if (needs_control_head)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);

  if (needs_control_head)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(2), TMP1, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP2, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_IMM, 0);
    }
  else
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP1, 0);

  init_frame(common, ccbegin, NULL, framesize + extrasize - 1, extrasize);
  }

memset(&altbacktrack, 0, sizeof(backtrack_common));
if (conditional || (opcode == OP_ASSERT_NOT || opcode == OP_ASSERTBACK_NOT))
  {
  /* Control verbs cannot escape from these asserts. */
  local_quit_available = TRUE;
  common->local_quit_available = TRUE;
  common->quit_label = NULL;
  common->quit = NULL;
  }

common->in_positive_assertion = (opcode == OP_ASSERT || opcode == OP_ASSERTBACK);
common->positive_assertion_quit = NULL;

while (1)
  {
  common->accept_label = NULL;
  common->accept = NULL;
  altbacktrack.top = NULL;
  altbacktrack.topbacktracks = NULL;

  if (*ccbegin == OP_ALT && extrasize > 0)
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));

  altbacktrack.cc = ccbegin;
  compile_matchingpath(common, ccbegin + 1 + LINK_SIZE, cc, &altbacktrack);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    {
    if (local_quit_available)
      {
      common->local_quit_available = save_local_quit_available;
      common->quit_label = save_quit_label;
      common->quit = save_quit;
      }
    common->in_positive_assertion = save_in_positive_assertion;
    common->then_trap = save_then_trap;
    common->accept_label = save_accept_label;
    common->positive_assertion_quit = save_positive_assertion_quit;
    common->accept = save_accept;
    return NULL;
    }
  common->accept_label = LABEL();
  if (common->accept != NULL)
    set_jumps(common->accept, common->accept_label);

  /* Reset stack. */
  if (framesize < 0)
    {
    if (framesize == no_frame)
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    else if (extrasize > 0)
      free_stack(common, extrasize);

    if (needs_control_head)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(-1));
    }
  else
    {
    if ((opcode != OP_ASSERT_NOT && opcode != OP_ASSERTBACK_NOT) || conditional)
      {
      /* We don't need to keep the STR_PTR, only the previous private_data_ptr. */
      OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, (framesize + 1) * sizeof(sljit_sw));
      if (needs_control_head)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(-1));
      }
    else
      {
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      if (needs_control_head)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(-framesize - 2));
      add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
      OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (framesize - 1) * sizeof(sljit_sw));
      }
    }

  if (opcode == OP_ASSERT_NOT || opcode == OP_ASSERTBACK_NOT)
    {
    /* We know that STR_PTR was stored on the top of the stack. */
    if (conditional)
      {
      if (extrasize > 0)
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), needs_control_head ? STACK(-2) : STACK(-1));
      }
    else if (bra == OP_BRAZERO)
      {
      if (framesize < 0)
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(-extrasize));
      else
        {
        OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-framesize - 1));
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(-framesize - extrasize));
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
        }
      OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    else if (framesize >= 0)
      {
      /* For OP_BRA and OP_BRAMINZERO. */
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_MEM1(STACK_TOP), STACK(-framesize - 1));
      }
    }
  add_jump(compiler, found, JUMP(SLJIT_JUMP));

  compile_backtrackingpath(common, altbacktrack.top);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    {
    if (local_quit_available)
      {
      common->local_quit_available = save_local_quit_available;
      common->quit_label = save_quit_label;
      common->quit = save_quit;
      }
    common->in_positive_assertion = save_in_positive_assertion;
    common->then_trap = save_then_trap;
    common->accept_label = save_accept_label;
    common->positive_assertion_quit = save_positive_assertion_quit;
    common->accept = save_accept;
    return NULL;
    }
  set_jumps(altbacktrack.topbacktracks, LABEL());

  if (*cc != OP_ALT)
    break;

  ccbegin = cc;
  cc += GET(cc, 1);
  }

if (local_quit_available)
  {
  SLJIT_ASSERT(common->positive_assertion_quit == NULL);
  /* Makes the check less complicated below. */
  common->positive_assertion_quit = common->quit;
  }

/* None of them matched. */
if (common->positive_assertion_quit != NULL)
  {
  jump = JUMP(SLJIT_JUMP);
  set_jumps(common->positive_assertion_quit, LABEL());
  SLJIT_ASSERT(framesize != no_stack);
  if (framesize < 0)
    OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, extrasize * sizeof(sljit_sw));
  else
    {
    OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
    add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
    OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (extrasize + 1) * sizeof(sljit_sw));
    }
  JUMPHERE(jump);
  }

if (needs_control_head)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(1));

if (opcode == OP_ASSERT || opcode == OP_ASSERTBACK)
  {
  /* Assert is failed. */
  if ((conditional && extrasize > 0) || bra == OP_BRAZERO)
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));

  if (framesize < 0)
    {
    /* The topmost item should be 0. */
    if (bra == OP_BRAZERO)
      {
      if (extrasize == 2)
        free_stack(common, 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    else if (extrasize > 0)
      free_stack(common, extrasize);
    }
  else
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(extrasize - 1));
    /* The topmost item should be 0. */
    if (bra == OP_BRAZERO)
      {
      free_stack(common, framesize + extrasize - 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    else
      free_stack(common, framesize + extrasize);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
    }
  jump = JUMP(SLJIT_JUMP);
  if (bra != OP_BRAZERO)
    add_jump(compiler, target, jump);

  /* Assert is successful. */
  set_jumps(tmp, LABEL());
  if (framesize < 0)
    {
    /* We know that STR_PTR was stored on the top of the stack. */
    if (extrasize > 0)
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(-extrasize));

    /* Keep the STR_PTR on the top of the stack. */
    if (bra == OP_BRAZERO)
      {
      OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
      if (extrasize == 2)
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
      }
    else if (bra == OP_BRAMINZERO)
      {
      OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    }
  else
    {
    if (bra == OP_BRA)
      {
      /* We don't need to keep the STR_PTR, only the previous private_data_ptr. */
      OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, (framesize + 1) * sizeof(sljit_sw));
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(-extrasize + 1));
      }
    else
      {
      /* We don't need to keep the STR_PTR, only the previous private_data_ptr. */
      OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, (framesize + 2) * sizeof(sljit_sw));
      if (extrasize == 2)
        {
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
        if (bra == OP_BRAMINZERO)
          OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
        }
      else
        {
        SLJIT_ASSERT(extrasize == 3);
        OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(-1));
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), bra == OP_BRAZERO ? STR_PTR : SLJIT_IMM, 0);
        }
      }
    }

  if (bra == OP_BRAZERO)
    {
    backtrack->matchingpath = LABEL();
    SET_LABEL(jump, backtrack->matchingpath);
    }
  else if (bra == OP_BRAMINZERO)
    {
    JUMPTO(SLJIT_JUMP, backtrack->matchingpath);
    JUMPHERE(brajump);
    if (framesize >= 0)
      {
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      add_jump(compiler, &common->revertframes, JUMP(SLJIT_FAST_CALL));
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(-2));
      OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, (framesize - 1) * sizeof(sljit_sw));
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
      }
    set_jumps(backtrack->common.topbacktracks, LABEL());
    }
  }
else
  {
  /* AssertNot is successful. */
  if (framesize < 0)
    {
    if (extrasize > 0)
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));

    if (bra != OP_BRA)
      {
      if (extrasize == 2)
        free_stack(common, 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    else if (extrasize > 0)
      free_stack(common, extrasize);
    }
  else
    {
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(extrasize - 1));
    /* The topmost item should be 0. */
    if (bra != OP_BRA)
      {
      free_stack(common, framesize + extrasize - 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    else
      free_stack(common, framesize + extrasize);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, TMP1, 0);
    }

  if (bra == OP_BRAZERO)
    backtrack->matchingpath = LABEL();
  else if (bra == OP_BRAMINZERO)
    {
    JUMPTO(SLJIT_JUMP, backtrack->matchingpath);
    JUMPHERE(brajump);
    }

  if (bra != OP_BRA)
    {
    SLJIT_ASSERT(found == &backtrack->common.topbacktracks);
    set_jumps(backtrack->common.topbacktracks, LABEL());
    backtrack->common.topbacktracks = NULL;
    }
  }

if (local_quit_available)
  {
  common->local_quit_available = save_local_quit_available;
  common->quit_label = save_quit_label;
  common->quit = save_quit;
  }
common->in_positive_assertion = save_in_positive_assertion;
common->then_trap = save_then_trap;
common->accept_label = save_accept_label;
common->positive_assertion_quit = save_positive_assertion_quit;
common->accept = save_accept;
return cc + 1 + LINK_SIZE;
}