static SLJIT_INLINE PCRE2_SPTR compile_recurse_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
recurse_entry *entry = common->entries;
recurse_entry *prev = NULL;
sljit_sw start = GET(cc, 1);
PCRE2_SPTR start_cc;
BOOL needs_control_head;

PUSH_BACKTRACK(sizeof(recurse_backtrack), cc, NULL);

/* Inlining simple patterns. */
if (get_framesize(common, common->start + start, NULL, TRUE, &needs_control_head) == no_stack)
  {
  start_cc = common->start + start;
  compile_matchingpath(common, next_opcode(common, start_cc), bracketend(start_cc) - (1 + LINK_SIZE), backtrack);
  BACKTRACK_AS(recurse_backtrack)->inlined_pattern = TRUE;
  return cc + 1 + LINK_SIZE;
  }

while (entry != NULL)
  {
  if (entry->start == start)
    break;
  prev = entry;
  entry = entry->next;
  }

if (entry == NULL)
  {
  entry = sljit_alloc_memory(compiler, sizeof(recurse_entry));
  if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
    return NULL;
  entry->next = NULL;
  entry->entry_label = NULL;
  entry->backtrack_label = NULL;
  entry->entry_calls = NULL;
  entry->backtrack_calls = NULL;
  entry->start = start;

  if (prev != NULL)
    prev->next = entry;
  else
    common->entries = entry;
  }

BACKTRACK_AS(recurse_backtrack)->entry = entry;

if (entry->entry_label == NULL)
  add_jump(compiler, &entry->entry_calls, JUMP(SLJIT_FAST_CALL));
else
  JUMPTO(SLJIT_FAST_CALL, entry->entry_label);
/* Leave if the match is failed. */
add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, 0));
BACKTRACK_AS(recurse_backtrack)->matchingpath = LABEL();
return cc + 1 + LINK_SIZE;
}