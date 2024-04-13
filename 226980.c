void imap_pretty_mailbox(char *path, size_t pathlen, const char *folder)
{
  struct ConnAccount cac_target = { { 0 } };
  struct ConnAccount cac_home = { { 0 } };
  struct Url url = { 0 };
  const char *delim = NULL;
  int tlen;
  int hlen = 0;
  bool home_match = false;
  char target_mailbox[1024];
  char home_mailbox[1024];

  if (imap_parse_path(path, &cac_target, target_mailbox, sizeof(target_mailbox)) < 0)
    return;

  if (imap_path_probe(folder, NULL) != MUTT_IMAP)
    goto fallback;

  if (imap_parse_path(folder, &cac_home, home_mailbox, sizeof(home_mailbox)) < 0)
    goto fallback;

  tlen = mutt_str_len(target_mailbox);
  hlen = mutt_str_len(home_mailbox);

  /* check whether we can do '+' substitution */
  if (tlen && imap_account_match(&cac_home, &cac_target) &&
      mutt_strn_equal(home_mailbox, target_mailbox, hlen))
  {
    const char *const c_imap_delim_chars =
        cs_subset_string(NeoMutt->sub, "imap_delim_chars");
    if (hlen == 0)
      home_match = true;
    else if (c_imap_delim_chars)
    {
      for (delim = c_imap_delim_chars; *delim != '\0'; delim++)
        if (target_mailbox[hlen] == *delim)
          home_match = true;
    }
  }

  /* do the '+' substitution */
  if (home_match)
  {
    *path++ = '+';
    /* copy remaining path, skipping delimiter */
    if (hlen == 0)
      hlen = -1;
    memcpy(path, target_mailbox + hlen + 1, tlen - hlen - 1);
    path[tlen - hlen - 1] = '\0';
    return;
  }

fallback:
  mutt_account_tourl(&cac_target, &url);
  url.path = target_mailbox;
  url_tostring(&url, path, pathlen, U_NO_FLAGS);
}