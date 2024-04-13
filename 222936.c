int imap_read_literal(FILE *fp, struct ImapAccountData *adata,
                      unsigned long bytes, struct Progress *pbar)
{
  char c;
  bool r = false;
  struct Buffer buf = { 0 }; // Do not allocate, maybe it won't be used

  if (C_DebugLevel >= IMAP_LOG_LTRL)
    mutt_buffer_alloc(&buf, bytes + 10);

  mutt_debug(LL_DEBUG2, "reading %ld bytes\n", bytes);

  for (unsigned long pos = 0; pos < bytes; pos++)
  {
    if (mutt_socket_readchar(adata->conn, &c) != 1)
    {
      mutt_debug(LL_DEBUG1, "error during read, %ld bytes read\n", pos);
      adata->status = IMAP_FATAL;

      mutt_buffer_dealloc(&buf);
      return -1;
    }

    if (r && (c != '\n'))
      fputc('\r', fp);

    if (c == '\r')
    {
      r = true;
      continue;
    }
    else
      r = false;

    fputc(c, fp);

    if (pbar && !(pos % 1024))
      mutt_progress_update(pbar, pos, -1);
    if (C_DebugLevel >= IMAP_LOG_LTRL)
      mutt_buffer_addch(&buf, c);
  }

  if (C_DebugLevel >= IMAP_LOG_LTRL)
  {
    mutt_debug(IMAP_LOG_LTRL, "\n%s", buf.data);
    mutt_buffer_dealloc(&buf);
  }
  return 0;
}