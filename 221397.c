static void cmd_parse_enabled (IMAP_DATA* idata, const char* s)
{
  dprint (2, (debugfile, "Handling ENABLED\n"));

  while ((s = imap_next_word ((char*)s)) && *s != '\0')
  {
    if (ascii_strncasecmp(s, "UTF8=ACCEPT", 11) == 0 ||
        ascii_strncasecmp(s, "UTF8=ONLY", 9) == 0)
      idata->unicode = 1;
  }
}