static sljit_sw SLJIT_FUNC do_search_mark(sljit_sw *current, PCRE2_SPTR skip_arg)
{
while (current != NULL)
  {
  switch (current[1])
    {
    case type_then_trap:
    break;

    case type_mark:
    if (PRIV(strcmp)(skip_arg, (PCRE2_SPTR)current[2]) == 0)
      return current[3];
    break;

    default:
    SLJIT_UNREACHABLE();
    break;
    }
  SLJIT_ASSERT(current[0] == 0 || current < (sljit_sw*)current[0]);
  current = (sljit_sw*)current[0];
  }
return 0;
}