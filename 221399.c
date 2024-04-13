static void cmd_parse_fetch (IMAP_DATA* idata, char* s)
{
  unsigned int msn, uid;
  HEADER *h;
  int server_changes = 0;

  dprint (3, (debugfile, "Handling FETCH\n"));

  if (mutt_atoui (s, &msn) < 0 ||
      msn < 1 || msn > idata->max_msn)
  {
    dprint (3, (debugfile, "FETCH response ignored for this message\n"));
    return;
  }

  h = idata->msn_index[msn - 1];
  if (!h || !h->active)
  {
    dprint (3, (debugfile, "FETCH response ignored for this message\n"));
    return;
  }

  dprint (2, (debugfile, "Message UID %u updated\n", HEADER_DATA(h)->uid));
  /* skip FETCH */
  s = imap_next_word (s);
  s = imap_next_word (s);

  if (*s != '(')
  {
    dprint (1, (debugfile, "Malformed FETCH response"));
    return;
  }
  s++;

  while (*s)
  {
    SKIPWS (s);

    if (ascii_strncasecmp ("FLAGS", s, 5) == 0)
    {
      imap_set_flags (idata, h, s, &server_changes);
      if (server_changes)
      {
        /* If server flags could conflict with mutt's flags, reopen the mailbox. */
        if (h->changed)
          idata->reopen |= IMAP_EXPUNGE_PENDING;
        else
          idata->check_status = IMAP_FLAGS_PENDING;
      }
      return;
    }
    else if (ascii_strncasecmp ("UID", s, 3) == 0)
    {
      s += 3;
      SKIPWS (s);
      if (mutt_atoui (s, &uid) < 0)
      {
        dprint (2, (debugfile, "Illegal UID.  Skipping update.\n"));
        return;
      }
      if (uid != HEADER_DATA(h)->uid)
      {
        dprint (2, (debugfile, "FETCH UID vs MSN mismatch.  Skipping update.\n"));
        return;
      }
      s = imap_next_word (s);
    }
    else if (*s == ')')
      s++; /* end of request */
    else if (*s)
    {
      dprint (2, (debugfile, "Only handle FLAGS updates\n"));
      return;
    }
  }
}