static PCRE2_SPTR compile_bracketpos_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
PCRE2_UCHAR opcode;
int private_data_ptr;
int cbraprivptr = 0;
BOOL needs_control_head;
int framesize;
int stacksize;
int offset = 0;
BOOL zero = FALSE;
PCRE2_SPTR ccbegin = NULL;
int stack; /* Also contains the offset of control head. */
struct sljit_label *loop = NULL;
struct jump_list *emptymatch = NULL;

PUSH_BACKTRACK(sizeof(bracketpos_backtrack), cc, NULL);
if (*cc == OP_BRAPOSZERO)
  {
  zero = TRUE;
  cc++;
  }

opcode = *cc;
private_data_ptr = PRIVATE_DATA(cc);
SLJIT_ASSERT(private_data_ptr != 0);
BACKTRACK_AS(bracketpos_backtrack)->private_data_ptr = private_data_ptr;
switch(opcode)
  {
  case OP_BRAPOS:
  case OP_SBRAPOS:
  ccbegin = cc + 1 + LINK_SIZE;
  break;

  case OP_CBRAPOS:
  case OP_SCBRAPOS:
  offset = GET2(cc, 1 + LINK_SIZE);
  /* This case cannot be optimized in the same was as
  normal capturing brackets. */
  SLJIT_ASSERT(common->optimized_cbracket[offset] == 0);
  cbraprivptr = OVECTOR_PRIV(offset);
  offset <<= 1;
  ccbegin = cc + 1 + LINK_SIZE + IMM2_SIZE;
  break;

  default:
  SLJIT_UNREACHABLE();
  break;
  }

framesize = get_framesize(common, cc, NULL, FALSE, &needs_control_head);
BACKTRACK_AS(bracketpos_backtrack)->framesize = framesize;
if (framesize < 0)
  {
  if (offset != 0)
    {
    stacksize = 2;
    if (common->capture_last_ptr != 0)
      stacksize++;
    }
  else
    stacksize = 1;

  if (needs_control_head)
    stacksize++;
  if (!zero)
    stacksize++;

  BACKTRACK_AS(bracketpos_backtrack)->stacksize = stacksize;
  allocate_stack(common, stacksize);
  if (framesize == no_frame)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STACK_TOP, 0);

  stack = 0;
  if (offset != 0)
    {
    stack = 2;
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP1, 0);
    if (common->capture_last_ptr != 0)
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP2, 0);
    if (needs_control_head)
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
    if (common->capture_last_ptr != 0)
      {
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(2), TMP1, 0);
      stack = 3;
      }
    }
  else
    {
    if (needs_control_head)
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    stack = 1;
    }

  if (needs_control_head)
    stack++;
  if (!zero)
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stack), SLJIT_IMM, 1);
  if (needs_control_head)
    {
    stack--;
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stack), TMP2, 0);
    }
  }
else
  {
  stacksize = framesize + 1;
  if (!zero)
    stacksize++;
  if (needs_control_head)
    stacksize++;
  if (offset == 0)
    stacksize++;
  BACKTRACK_AS(bracketpos_backtrack)->stacksize = stacksize;

  allocate_stack(common, stacksize);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
  if (needs_control_head)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr);
  OP2(SLJIT_ADD, SLJIT_MEM1(SLJIT_SP), private_data_ptr, STACK_TOP, 0, SLJIT_IMM, stacksize * sizeof(sljit_sw));

  stack = 0;
  if (!zero)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 1);
    stack = 1;
    }
  if (needs_control_head)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stack), TMP2, 0);
    stack++;
    }
  if (offset == 0)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stack), STR_PTR, 0);
    stack++;
    }
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stack), TMP1, 0);
  init_frame(common, cc, NULL, stacksize - 1, stacksize - framesize);
  stack -= 1 + (offset == 0);
  }

if (offset != 0)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), cbraprivptr, STR_PTR, 0);

loop = LABEL();
while (*cc != OP_KETRPOS)
  {
  backtrack->top = NULL;
  backtrack->topbacktracks = NULL;
  cc += GET(cc, 1);

  compile_matchingpath(common, ccbegin, cc, backtrack);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    return NULL;

  if (framesize < 0)
    {
    if (framesize == no_frame)
      OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);

    if (offset != 0)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), cbraprivptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), cbraprivptr, STR_PTR, 0);
      if (common->capture_last_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr, SLJIT_IMM, offset >> 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
      }
    else
      {
      if (opcode == OP_SBRAPOS)
        OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
      }

    /* Even if the match is empty, we need to reset the control head. */
    if (needs_control_head)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(stack));

    if (opcode == OP_SBRAPOS || opcode == OP_SCBRAPOS)
      add_jump(compiler, &emptymatch, CMP(SLJIT_EQUAL, TMP1, 0, STR_PTR, 0));

    if (!zero)
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize - 1), SLJIT_IMM, 0);
    }
  else
    {
    if (offset != 0)
      {
      OP2(SLJIT_SUB, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr, SLJIT_IMM, stacksize * sizeof(sljit_sw));
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), cbraprivptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), cbraprivptr, STR_PTR, 0);
      if (common->capture_last_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr, SLJIT_IMM, offset >> 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
      }
    else
      {
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      OP2(SLJIT_SUB, STACK_TOP, 0, TMP2, 0, SLJIT_IMM, stacksize * sizeof(sljit_sw));
      if (opcode == OP_SBRAPOS)
        OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), STACK(-framesize - 2));
      OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), STACK(-framesize - 2), STR_PTR, 0);
      }

    /* Even if the match is empty, we need to reset the control head. */
    if (needs_control_head)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_MEM1(STACK_TOP), STACK(stack));

    if (opcode == OP_SBRAPOS || opcode == OP_SCBRAPOS)
      add_jump(compiler, &emptymatch, CMP(SLJIT_EQUAL, TMP1, 0, STR_PTR, 0));

    if (!zero)
      {
      if (framesize < 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize - 1), SLJIT_IMM, 0);
      else
        OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
      }
    }

  JUMPTO(SLJIT_JUMP, loop);
  flush_stubs(common);

  compile_backtrackingpath(common, backtrack->top);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    return NULL;
  set_jumps(backtrack->topbacktracks, LABEL());

  if (framesize < 0)
    {
    if (offset != 0)
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), cbraprivptr);
    else
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    }
  else
    {
    if (offset != 0)
      {
      /* Last alternative. */
      if (*cc == OP_KETRPOS)
        OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), cbraprivptr);
      }
    else
      {
      OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
      OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(TMP2), STACK(-framesize - 2));
      }
    }

  if (*cc == OP_KETRPOS)
    break;
  ccbegin = cc + 1 + LINK_SIZE;
  }

/* We don't have to restore the control head in case of a failed match. */

backtrack->topbacktracks = NULL;
if (!zero)
  {
  if (framesize < 0)
    add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_NOT_EQUAL, SLJIT_MEM1(STACK_TOP), STACK(stacksize - 1), SLJIT_IMM, 0));
  else /* TMP2 is set to [private_data_ptr] above. */
    add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_NOT_EQUAL, SLJIT_MEM1(TMP2), STACK(-stacksize), SLJIT_IMM, 0));
  }

/* None of them matched. */
set_jumps(emptymatch, LABEL());
count_match(common);
return cc + 1 + LINK_SIZE;
}