int imap_code (const char *s)
{
  return cmd_status (s) == IMAP_CMD_OK;
}