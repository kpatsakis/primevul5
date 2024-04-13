void imap_cmd_finish (IMAP_DATA* idata)
{
  if (idata->status == IMAP_FATAL)
  {
    cmd_handle_fatal (idata);
    return;
  }

  if (!(idata->state >= IMAP_SELECTED) || idata->ctx->closing)
    return;
  
  if (idata->reopen & IMAP_REOPEN_ALLOW)
  {
    unsigned int count = idata->newMailCount;

    if (!(idata->reopen & IMAP_EXPUNGE_PENDING) &&
	(idata->reopen & IMAP_NEWMAIL_PENDING)
	&& count > idata->max_msn)
    {
      /* read new mail messages */
      dprint (2, (debugfile, "imap_cmd_finish: Fetching new mail\n"));
      /* check_status: curs_main uses imap_check_mailbox to detect
       *   whether the index needs updating */
      idata->check_status = IMAP_NEWMAIL_PENDING;
      imap_read_headers (idata, idata->max_msn+1, count);
    }
    else if (idata->reopen & IMAP_EXPUNGE_PENDING)
    {
      dprint (2, (debugfile, "imap_cmd_finish: Expunging mailbox\n"));
      imap_expunge_mailbox (idata);
      /* Detect whether we've gotten unexpected EXPUNGE messages */
      if ((idata->reopen & IMAP_EXPUNGE_PENDING) &&
	  !(idata->reopen & IMAP_EXPUNGE_EXPECTED))
	idata->check_status = IMAP_EXPUNGE_PENDING;
      idata->reopen &= ~(IMAP_EXPUNGE_PENDING | IMAP_NEWMAIL_PENDING |
			 IMAP_EXPUNGE_EXPECTED);
    }
  }

  idata->status = 0;
}