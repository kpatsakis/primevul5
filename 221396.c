static void cmd_parse_capability (IMAP_DATA* idata, char* s)
{
  int x;
  char* bracket;

  dprint (3, (debugfile, "Handling CAPABILITY\n"));

  s = imap_next_word (s);
  if ((bracket = strchr (s, ']')))
    *bracket = '\0';
  FREE(&idata->capstr);
  idata->capstr = safe_strdup (s);

  memset (idata->capabilities, 0, sizeof (idata->capabilities));

  while (*s)
  {
    for (x = 0; x < CAPMAX; x++)
      if (imap_wordcasecmp(Capabilities[x], s) == 0)
      {
	mutt_bit_set (idata->capabilities, x);
	break;
      }
    s = imap_next_word (s);
  }
}