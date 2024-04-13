int imap_complete(char *buf, size_t buflen, const char *path)
{
  struct ImapAccountData *adata = NULL;
  struct ImapMboxData *mdata = NULL;
  char tmp[2048];
  struct ImapList listresp = { 0 };
  char completion[1024];
  int clen;
  size_t matchlen = 0;
  int completions = 0;
  int rc;

  if (imap_adata_find(path, &adata, &mdata) < 0)
  {
    mutt_str_copy(buf, path, buflen);
    return complete_hosts(buf, buflen);
  }

  /* fire off command */
  snprintf(tmp, sizeof(tmp), "%s \"\" \"%s%%\"",
           C_ImapListSubscribed ? "LSUB" : "LIST", mdata->real_name);

  imap_cmd_start(adata, tmp);

  /* and see what the results are */
  mutt_str_copy(completion, mdata->name, sizeof(completion));
  imap_mdata_free((void *) &mdata);

  adata->cmdresult = &listresp;
  do
  {
    listresp.name = NULL;
    rc = imap_cmd_step(adata);

    if ((rc == IMAP_RES_CONTINUE) && listresp.name)
    {
      /* if the folder isn't selectable, append delimiter to force browse
       * to enter it on second tab. */
      if (listresp.noselect)
      {
        clen = strlen(listresp.name);
        listresp.name[clen++] = listresp.delim;
        listresp.name[clen] = '\0';
      }
      /* copy in first word */
      if (!completions)
      {
        mutt_str_copy(completion, listresp.name, sizeof(completion));
        matchlen = strlen(completion);
        completions++;
        continue;
      }

      matchlen = longest_common_prefix(completion, listresp.name, 0, matchlen);
      completions++;
    }
  } while (rc == IMAP_RES_CONTINUE);
  adata->cmdresult = NULL;

  if (completions)
  {
    /* reformat output */
    imap_qualify_path(buf, buflen, &adata->conn->account, completion);
    mutt_pretty_mailbox(buf, buflen);
    return 0;
  }

  return -1;
}