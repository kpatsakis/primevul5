int imap_cmd_start (IMAP_DATA* idata, const char* cmdstr)
{
  return cmd_start (idata, cmdstr, 0);
}