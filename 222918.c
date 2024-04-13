int imap_sync_message_for_copy(struct Mailbox *m, struct Email *e,
                               struct Buffer *cmd, enum QuadOption *err_continue)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  if (!adata || (adata->mailbox != m))
    return -1;

  char flags[1024];
  char *tags = NULL;
  char uid[11];

  if (!compare_flags_for_copy(e))
  {
    if (e->deleted == imap_edata_get(e)->deleted)
      e->changed = false;
    return 0;
  }

  snprintf(uid, sizeof(uid), "%u", imap_edata_get(e)->uid);
  mutt_buffer_reset(cmd);
  mutt_buffer_addstr(cmd, "UID STORE ");
  mutt_buffer_addstr(cmd, uid);

  flags[0] = '\0';

  set_flag(m, MUTT_ACL_SEEN, e->read, "\\Seen ", flags, sizeof(flags));
  set_flag(m, MUTT_ACL_WRITE, e->old, "Old ", flags, sizeof(flags));
  set_flag(m, MUTT_ACL_WRITE, e->flagged, "\\Flagged ", flags, sizeof(flags));
  set_flag(m, MUTT_ACL_WRITE, e->replied, "\\Answered ", flags, sizeof(flags));
  set_flag(m, MUTT_ACL_DELETE, imap_edata_get(e)->deleted, "\\Deleted ", flags,
           sizeof(flags));

  if (m->rights & MUTT_ACL_WRITE)
  {
    /* restore system flags */
    if (imap_edata_get(e)->flags_system)
      mutt_str_cat(flags, sizeof(flags), imap_edata_get(e)->flags_system);
    /* set custom flags */
    tags = driver_tags_get_with_hidden(&e->tags);
    if (tags)
    {
      mutt_str_cat(flags, sizeof(flags), tags);
      FREE(&tags);
    }
  }

  mutt_str_remove_trailing_ws(flags);

  /* UW-IMAP is OK with null flags, Cyrus isn't. The only solution is to
   * explicitly revoke all system flags (if we have permission) */
  if (*flags == '\0')
  {
    set_flag(m, MUTT_ACL_SEEN, 1, "\\Seen ", flags, sizeof(flags));
    set_flag(m, MUTT_ACL_WRITE, 1, "Old ", flags, sizeof(flags));
    set_flag(m, MUTT_ACL_WRITE, 1, "\\Flagged ", flags, sizeof(flags));
    set_flag(m, MUTT_ACL_WRITE, 1, "\\Answered ", flags, sizeof(flags));
    set_flag(m, MUTT_ACL_DELETE, !imap_edata_get(e)->deleted, "\\Deleted ",
             flags, sizeof(flags));

    /* erase custom flags */
    if ((m->rights & MUTT_ACL_WRITE) && imap_edata_get(e)->flags_remote)
      mutt_str_cat(flags, sizeof(flags), imap_edata_get(e)->flags_remote);

    mutt_str_remove_trailing_ws(flags);

    mutt_buffer_addstr(cmd, " -FLAGS.SILENT (");
  }
  else
    mutt_buffer_addstr(cmd, " FLAGS.SILENT (");

  mutt_buffer_addstr(cmd, flags);
  mutt_buffer_addstr(cmd, ")");

  /* after all this it's still possible to have no flags, if you
   * have no ACL rights */
  if (*flags && (imap_exec(adata, cmd->data, IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS) &&
      err_continue && (*err_continue != MUTT_YES))
  {
    *err_continue = imap_continue("imap_sync_message: STORE failed", adata->buf);
    if (*err_continue != MUTT_YES)
      return -1;
  }

  /* server have now the updated flags */
  FREE(&imap_edata_get(e)->flags_remote);
  imap_edata_get(e)->flags_remote = driver_tags_get_with_hidden(&e->tags);

  if (e->deleted == imap_edata_get(e)->deleted)
    e->changed = false;

  return 0;
}