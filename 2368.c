static SLJIT_INLINE PCRE2_SPTR set_then_offsets(compiler_common *common, PCRE2_SPTR cc, sljit_u8 *current_offset)
{
PCRE2_SPTR end = bracketend(cc);
BOOL has_alternatives = cc[GET(cc, 1)] == OP_ALT;

/* Assert captures then. */
if (*cc >= OP_ASSERT && *cc <= OP_ASSERTBACK_NA)
  current_offset = NULL;
/* Conditional block does not. */
if (*cc == OP_COND || *cc == OP_SCOND)
  has_alternatives = FALSE;

cc = next_opcode(common, cc);
if (has_alternatives)
  current_offset = common->then_offsets + (cc - common->start);

while (cc < end)
  {
  if ((*cc >= OP_ASSERT && *cc <= OP_ASSERTBACK_NA) || (*cc >= OP_ONCE && *cc <= OP_SCOND))
    cc = set_then_offsets(common, cc, current_offset);
  else
    {
    if (*cc == OP_ALT && has_alternatives)
      current_offset = common->then_offsets + (cc + 1 + LINK_SIZE - common->start);
    if (*cc >= OP_THEN && *cc <= OP_THEN_ARG && current_offset != NULL)
      *current_offset = 1;
    cc = next_opcode(common, cc);
    }
  }

return end;
}