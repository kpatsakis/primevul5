int imap_parse_path(const char *path, struct ConnAccount *cac, char *mailbox, size_t mailboxlen)
{
  static unsigned short ImapPort = 0;
  static unsigned short ImapsPort = 0;

  if (ImapPort == 0)
  {
    struct servent *service = getservbyname("imap", "tcp");
    if (service)
      ImapPort = ntohs(service->s_port);
    else
      ImapPort = IMAP_PORT;
    mutt_debug(LL_DEBUG3, "Using default IMAP port %d\n", ImapPort);
  }

  if (ImapsPort == 0)
  {
    struct servent *service = getservbyname("imaps", "tcp");
    if (service)
      ImapsPort = ntohs(service->s_port);
    else
      ImapsPort = IMAP_SSL_PORT;
    mutt_debug(LL_DEBUG3, "Using default IMAPS port %d\n", ImapsPort);
  }

  /* Defaults */
  cac->port = ImapPort;
  cac->type = MUTT_ACCT_TYPE_IMAP;
  cac->service = "imap";
  cac->get_field = imap_get_field;

  struct Url *url = url_parse(path);
  if (!url)
    return -1;

  if ((url->scheme != U_IMAP) && (url->scheme != U_IMAPS))
  {
    url_free(&url);
    return -1;
  }

  if ((mutt_account_fromurl(cac, url) < 0) || (cac->host[0] == '\0'))
  {
    url_free(&url);
    return -1;
  }

  if (url->scheme == U_IMAPS)
    cac->flags |= MUTT_ACCT_SSL;

  mutt_str_copy(mailbox, url->path, mailboxlen);

  url_free(&url);

  if ((cac->flags & MUTT_ACCT_SSL) && !(cac->flags & MUTT_ACCT_PORT))
    cac->port = ImapsPort;

  return 0;
}