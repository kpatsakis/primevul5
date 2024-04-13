bool imap_has_flag(struct ListHead *flag_list, const char *flag)
{
  if (STAILQ_EMPTY(flag_list))
    return false;

  const size_t flaglen = mutt_str_len(flag);
  struct ListNode *np = NULL;
  STAILQ_FOREACH(np, flag_list, entries)
  {
    const size_t nplen = strlen(np->data);
    if ((flaglen >= nplen) && ((flag[nplen] == '\0') || (flag[nplen] == ' ')) &&
        mutt_istrn_equal(np->data, flag, nplen))
    {
      return true;
    }

    if (mutt_str_equal(np->data, "\\*"))
      return true;
  }

  return false;
}