static SLJIT_INLINE void compile_recurse_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
recurse_entry *entry;

if (!CURRENT_AS(recurse_backtrack)->inlined_pattern)
  {
  entry = CURRENT_AS(recurse_backtrack)->entry;
  if (entry->backtrack_label == NULL)
    add_jump(compiler, &entry->backtrack_calls, JUMP(SLJIT_FAST_CALL));
  else
    JUMPTO(SLJIT_FAST_CALL, entry->backtrack_label);
  CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, 0, CURRENT_AS(recurse_backtrack)->matchingpath);
  }
else
  compile_backtrackingpath(common, current->top);

set_jumps(current->topbacktracks, LABEL());
}