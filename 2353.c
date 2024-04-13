static SLJIT_INLINE void compile_braminzero_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
assert_backtrack backtrack;

current->top = NULL;
current->topbacktracks = NULL;
current->nextbacktracks = NULL;
if (current->cc[1] > OP_ASSERTBACK_NOT)
  {
  /* Manual call of compile_bracket_matchingpath and compile_bracket_backtrackingpath. */
  compile_bracket_matchingpath(common, current->cc, current);
  compile_bracket_backtrackingpath(common, current->top);
  }
else
  {
  memset(&backtrack, 0, sizeof(backtrack));
  backtrack.common.cc = current->cc;
  backtrack.matchingpath = CURRENT_AS(braminzero_backtrack)->matchingpath;
  /* Manual call of compile_assert_matchingpath. */
  compile_assert_matchingpath(common, current->cc, &backtrack, FALSE);
  }
SLJIT_ASSERT(!current->nextbacktracks && !current->topbacktracks);
}