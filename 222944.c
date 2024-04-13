static int complete_hosts(char *buf, size_t buflen)
{
  // struct Connection *conn = NULL;
  int rc = -1;
  size_t matchlen;

  matchlen = mutt_str_len(buf);
  struct MailboxList ml = STAILQ_HEAD_INITIALIZER(ml);
  neomutt_mailboxlist_get_all(&ml, NeoMutt, MUTT_MAILBOX_ANY);
  struct MailboxNode *np = NULL;
  STAILQ_FOREACH(np, &ml, entries)
  {
    if (!mutt_str_startswith(mailbox_path(np->mailbox), buf))
      continue;

    if (rc)
    {
      mutt_str_copy(buf, mailbox_path(np->mailbox), buflen);
      rc = 0;
    }
    else
      longest_common_prefix(buf, mailbox_path(np->mailbox), matchlen, buflen);
  }
  neomutt_mailboxlist_clear(&ml);

#if 0
  TAILQ_FOREACH(conn, mutt_socket_head(), entries)
  {
    struct Url url = { 0 };
    char urlstr[1024];

    if (conn->account.type != MUTT_ACCT_TYPE_IMAP)
      continue;

    mutt_account_tourl(&conn->account, &url);
    /* FIXME: how to handle multiple users on the same host? */
    url.user = NULL;
    url.path = NULL;
    url_tostring(&url, urlstr, sizeof(urlstr), 0);
    if (mutt_strn_equal(buf, urlstr, matchlen))
    {
      if (rc)
      {
        mutt_str_copy(buf, urlstr, buflen);
        rc = 0;
      }
      else
        longest_common_prefix(buf, urlstr, matchlen, buflen);
    }
  }
#endif

  return rc;
}