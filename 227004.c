bool imap_account_match(const struct ConnAccount *a1, const struct ConnAccount *a2)
{
  if (!a1 || !a2)
    return false;
  if (a1->type != a2->type)
    return false;
  if (!mutt_istr_equal(a1->host, a2->host))
    return false;
  if ((a1->port != 0) && (a2->port != 0) && (a1->port != a2->port))
    return false;
  if (a1->flags & a2->flags & MUTT_ACCT_USER)
    return strcmp(a1->user, a2->user) == 0;

  const char *user = NONULL(Username);

  const char *const c_imap_user = cs_subset_string(NeoMutt->sub, "imap_user");
  if ((a1->type == MUTT_ACCT_TYPE_IMAP) && c_imap_user)
    user = c_imap_user;

  if (a1->flags & MUTT_ACCT_USER)
    return strcmp(a1->user, user) == 0;
  if (a2->flags & MUTT_ACCT_USER)
    return strcmp(a2->user, user) == 0;

  return true;
}