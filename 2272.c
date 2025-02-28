static SLJIT_INLINE void compile_recurse(compiler_common *common)
{
DEFINE_COMPILER;
PCRE2_SPTR cc = common->start + common->currententry->start;
PCRE2_SPTR ccbegin = cc + 1 + LINK_SIZE + (*cc == OP_BRA ? 0 : IMM2_SIZE);
PCRE2_SPTR ccend = bracketend(cc) - (1 + LINK_SIZE);
BOOL needs_control_head;
BOOL has_quit;
BOOL has_accept;
int private_data_size = get_recurse_data_length(common, ccbegin, ccend, &needs_control_head, &has_quit, &has_accept);
int alt_count, alt_max, local_size;
backtrack_common altbacktrack;
jump_list *match = NULL;
struct sljit_jump *next_alt = NULL;
struct sljit_jump *accept_exit = NULL;
struct sljit_label *quit;
struct sljit_put_label *put_label = NULL;

/* Recurse captures then. */
common->then_trap = NULL;

SLJIT_ASSERT(*cc == OP_BRA || *cc == OP_CBRA || *cc == OP_CBRAPOS || *cc == OP_SCBRA || *cc == OP_SCBRAPOS);

alt_max = no_alternatives(cc);
alt_count = 0;

/* Matching path. */
SLJIT_ASSERT(common->currententry->entry_label == NULL && common->recursive_head_ptr != 0);
common->currententry->entry_label = LABEL();
set_jumps(common->currententry->entry_calls, common->currententry->entry_label);

sljit_emit_fast_enter(compiler, TMP2, 0);
count_match(common);

local_size = (alt_max > 1) ? 2 : 1;

/* (Reversed) stack layout:
   [private data][return address][optional: str ptr] ... [optional: alternative index][recursive_head_ptr] */

allocate_stack(common, private_data_size + local_size);
/* Save return address. */
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(local_size - 1), TMP2, 0);

copy_recurse_data(common, ccbegin, ccend, recurse_copy_from_global, local_size, private_data_size + local_size, has_quit);

/* This variable is saved and restored all time when we enter or exit from a recursive context. */
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->recursive_head_ptr, STACK_TOP, 0);

if (needs_control_head)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->control_head_ptr, SLJIT_IMM, 0);

if (alt_max > 1)
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);

memset(&altbacktrack, 0, sizeof(backtrack_common));
common->quit_label = NULL;
common->accept_label = NULL;
common->quit = NULL;
common->accept = NULL;
altbacktrack.cc = ccbegin;
cc += GET(cc, 1);
while (1)
  {
  altbacktrack.top = NULL;
  altbacktrack.topbacktracks = NULL;

  if (altbacktrack.cc != ccbegin)
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));

  compile_matchingpath(common, altbacktrack.cc, cc, &altbacktrack);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    return;

  allocate_stack(common, (alt_max > 1 || has_accept) ? 2 : 1);
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->recursive_head_ptr);

  if (alt_max > 1 || has_accept)
    {
    if (alt_max > 3)
      put_label = sljit_emit_put_label(compiler, SLJIT_MEM1(STACK_TOP), STACK(1));
    else
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, alt_count);
    }

  add_jump(compiler, &match, JUMP(SLJIT_JUMP));

  if (alt_count == 0)
    {
    /* Backtracking path entry. */
    SLJIT_ASSERT(common->currententry->backtrack_label == NULL);
    common->currententry->backtrack_label = LABEL();
    set_jumps(common->currententry->backtrack_calls, common->currententry->backtrack_label);

    sljit_emit_fast_enter(compiler, TMP1, 0);

    if (has_accept)
      accept_exit = CMP(SLJIT_EQUAL, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, -1);

    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    /* Save return address. */
    OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), STACK(local_size - 1), TMP1, 0);

    copy_recurse_data(common, ccbegin, ccend, recurse_swap_global, local_size, private_data_size + local_size, has_quit);

    if (alt_max > 1)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
      free_stack(common, 2);

      if (alt_max > 3)
        {
        sljit_emit_ijump(compiler, SLJIT_JUMP, TMP1, 0);
        sljit_set_put_label(put_label, LABEL());
        sljit_emit_op0(compiler, SLJIT_ENDBR);
        }
      else
        next_alt = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0);
      }
    else
      free_stack(common, has_accept ? 2 : 1);
    }
  else if (alt_max > 3)
    {
    sljit_set_put_label(put_label, LABEL());
    sljit_emit_op0(compiler, SLJIT_ENDBR);
    }
  else
    {
    JUMPHERE(next_alt);
    if (alt_count + 1 < alt_max)
      {
      SLJIT_ASSERT(alt_count == 1 && alt_max == 3);
      next_alt = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 1);
      }
    }

  alt_count++;

  compile_backtrackingpath(common, altbacktrack.top);
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    return;
  set_jumps(altbacktrack.topbacktracks, LABEL());

  if (*cc != OP_ALT)
    break;

  altbacktrack.cc = cc + 1 + LINK_SIZE;
  cc += GET(cc, 1);
  }

/* No alternative is matched. */

quit = LABEL();

copy_recurse_data(common, ccbegin, ccend, recurse_copy_private_to_global, local_size, private_data_size + local_size, has_quit);

OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(local_size - 1));
free_stack(common, private_data_size + local_size);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
OP_SRC(SLJIT_FAST_RETURN, TMP2, 0);

if (common->quit != NULL)
  {
  SLJIT_ASSERT(has_quit);

  set_jumps(common->quit, LABEL());
  OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), common->recursive_head_ptr);
  copy_recurse_data(common, ccbegin, ccend, recurse_copy_shared_to_global, local_size, private_data_size + local_size, has_quit);
  JUMPTO(SLJIT_JUMP, quit);
  }

if (has_accept)
  {
  JUMPHERE(accept_exit);
  free_stack(common, 2);

  /* Save return address. */
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(local_size - 1), TMP1, 0);

  copy_recurse_data(common, ccbegin, ccend, recurse_copy_kept_shared_to_global, local_size, private_data_size + local_size, has_quit);

  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(local_size - 1));
  free_stack(common, private_data_size + local_size);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 0);
  OP_SRC(SLJIT_FAST_RETURN, TMP2, 0);
  }

if (common->accept != NULL)
  {
  SLJIT_ASSERT(has_accept);

  set_jumps(common->accept, LABEL());

  OP1(SLJIT_MOV, STACK_TOP, 0, SLJIT_MEM1(SLJIT_SP), common->recursive_head_ptr);
  OP1(SLJIT_MOV, TMP2, 0, STACK_TOP, 0);

  allocate_stack(common, 2);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, -1);
  }

set_jumps(match, LABEL());

OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), TMP2, 0);

copy_recurse_data(common, ccbegin, ccend, recurse_swap_global, local_size, private_data_size + local_size, has_quit);

OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP2), STACK(local_size - 1));
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 1);
OP_SRC(SLJIT_FAST_RETURN, TMP2, 0);
}