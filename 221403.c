static void cmd_parse_list (IMAP_DATA* idata, char* s)
{
  IMAP_LIST* list;
  IMAP_LIST lb;
  char delimbuf[5]; /* worst case: "\\"\0 */
  unsigned int litlen;

  if (idata->cmddata && idata->cmdtype == IMAP_CT_LIST)
    list = (IMAP_LIST*)idata->cmddata;
  else
    list = &lb;

  memset (list, 0, sizeof (IMAP_LIST));

  /* flags */
  s = imap_next_word (s);
  if (*s != '(')
  {
    dprint (1, (debugfile, "Bad LIST response\n"));
    return;
  }
  s++;
  while (*s)
  {
    if (!ascii_strncasecmp (s, "\\NoSelect", 9))
      list->noselect = 1;
    else if (!ascii_strncasecmp (s, "\\NoInferiors", 12))
      list->noinferiors = 1;
    /* See draft-gahrns-imap-child-mailbox-?? */
    else if (!ascii_strncasecmp (s, "\\HasNoChildren", 14))
      list->noinferiors = 1;
    
    s = imap_next_word (s);
    if (*(s - 2) == ')')
      break;
  }

  /* Delimiter */
  if (ascii_strncasecmp (s, "NIL", 3))
  {
    delimbuf[0] = '\0';
    safe_strcat (delimbuf, 5, s); 
    imap_unquote_string (delimbuf);
    list->delim = delimbuf[0];
  }

  /* Name */
  s = imap_next_word (s);
  /* Notes often responds with literals here. We need a real tokenizer. */
  if (!imap_get_literal_count (s, &litlen))
  {
    if (imap_cmd_step (idata) != IMAP_CMD_CONTINUE)
    {
      idata->status = IMAP_FATAL;
      return;
    }
    list->name = idata->buf;
  }
  else
  {
    imap_unmunge_mbox_name (idata, s);
    list->name = s;
  }

  if (list->name[0] == '\0')
  {
    idata->delim = list->delim;
    dprint (3, (debugfile, "Root delimiter: %c\n", idata->delim));
  }
}