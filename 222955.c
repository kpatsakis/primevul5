int imap_fast_trash(struct Mailbox *m, char *dest)
{
  char prompt[1024];
  int rc = -1;
  bool triedcreate = false;
  enum QuadOption err_continue = MUTT_NO;

  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapAccountData *dest_adata = NULL;
  struct ImapMboxData *dest_mdata = NULL;

  if (imap_adata_find(dest, &dest_adata, &dest_mdata) < 0)
    return -1;

  struct Buffer sync_cmd = mutt_buffer_make(0);

  /* check that the save-to folder is in the same account */
  if (!imap_account_match(&(adata->conn->account), &(dest_adata->conn->account)))
  {
    mutt_debug(LL_DEBUG3, "%s not same server as %s\n", dest, mailbox_path(m));
    goto out;
  }

  for (int i = 0; i < m->msg_count; i++)
  {
    struct Email *e = m->emails[i];
    if (!e)
      break;
    if (e->active && e->changed && e->deleted && !e->purge)
    {
      rc = imap_sync_message_for_copy(m, e, &sync_cmd, &err_continue);
      if (rc < 0)
      {
        mutt_debug(LL_DEBUG1, "could not sync\n");
        goto out;
      }
    }
  }

  /* loop in case of TRYCREATE */
  do
  {
    rc = imap_exec_msgset(m, "UID COPY", dest_mdata->munge_name, MUTT_TRASH, false, false);
    if (rc == 0)
    {
      mutt_debug(LL_DEBUG1, "No messages to trash\n");
      rc = -1;
      goto out;
    }
    else if (rc < 0)
    {
      mutt_debug(LL_DEBUG1, "could not queue copy\n");
      goto out;
    }
    else if (m->verbose)
    {
      mutt_message(ngettext("Copying %d message to %s...", "Copying %d messages to %s...", rc),
                   rc, dest_mdata->name);
    }

    /* let's get it on */
    rc = imap_exec(adata, NULL, IMAP_CMD_NO_FLAGS);
    if (rc == IMAP_EXEC_ERROR)
    {
      if (triedcreate)
      {
        mutt_debug(LL_DEBUG1, "Already tried to create mailbox %s\n", dest_mdata->name);
        break;
      }
      /* bail out if command failed for reasons other than nonexistent target */
      if (!mutt_istr_startswith(imap_get_qualifier(adata->buf), "[TRYCREATE]"))
        break;
      mutt_debug(LL_DEBUG3, "server suggests TRYCREATE\n");
      snprintf(prompt, sizeof(prompt), _("Create %s?"), dest_mdata->name);
      if (C_Confirmcreate && (mutt_yesorno(prompt, MUTT_YES) != MUTT_YES))
      {
        mutt_clear_error();
        goto out;
      }
      if (imap_create_mailbox(adata, dest_mdata->name) < 0)
        break;
      triedcreate = true;
    }
  } while (rc == IMAP_EXEC_ERROR);

  if (rc != IMAP_EXEC_SUCCESS)
  {
    imap_error("imap_fast_trash", adata->buf);
    goto out;
  }

  rc = IMAP_EXEC_SUCCESS;

out:
  mutt_buffer_dealloc(&sync_cmd);
  imap_mdata_free((void *) &dest_mdata);

  return ((rc == IMAP_EXEC_SUCCESS) ? 0 : -1);
}