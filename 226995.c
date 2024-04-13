int imap_mxcmp(const char *mx1, const char *mx2)
{
  char *b1 = NULL;
  char *b2 = NULL;
  int rc;

  if (!mx1 || (*mx1 == '\0'))
    mx1 = "INBOX";
  if (!mx2 || (*mx2 == '\0'))
    mx2 = "INBOX";
  if (mutt_istr_equal(mx1, "INBOX") && mutt_istr_equal(mx2, "INBOX"))
  {
    return 0;
  }

  b1 = mutt_mem_malloc(strlen(mx1) + 1);
  b2 = mutt_mem_malloc(strlen(mx2) + 1);

  imap_fix_path('\0', mx1, b1, strlen(mx1) + 1);
  imap_fix_path('\0', mx2, b2, strlen(mx2) + 1);

  rc = mutt_str_cmp(b1, b2);
  FREE(&b1);
  FREE(&b2);

  return rc;
}