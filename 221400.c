static int cmd_status (const char *s)
{
  s = imap_next_word((char*)s);
  
  if (!ascii_strncasecmp("OK", s, 2))
    return IMAP_CMD_OK;
  if (!ascii_strncasecmp("NO", s, 2))
    return IMAP_CMD_NO;

  return IMAP_CMD_BAD;
}