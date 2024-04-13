static void cmd_parse_search (IMAP_DATA* idata, const char* s)
{
  unsigned int uid;
  HEADER *h;

  dprint (2, (debugfile, "Handling SEARCH\n"));

  while ((s = imap_next_word ((char*)s)) && *s != '\0')
  {
    if (mutt_atoui (s, &uid) < 0)
      continue;
    h = (HEADER *)int_hash_find (idata->uid_hash, uid);
    if (h)
      h->matched = 1;
  }
}