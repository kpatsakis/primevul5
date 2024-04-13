int newsgroups_cb(const char *mailbox,
		  uint32_t uid __attribute__((unused)),
		  const char *entry __attribute__((unused)),
		  const char *userid,
		  const struct buf *value, void *rock)
{
    struct wildmat *wild = (struct wildmat *) rock;

    /* skip personal mailboxes */
    if ((!strncasecmp(mailbox, "INBOX", 5) &&
	 (!mailbox[5] || mailbox[5] == '.')) ||
	!strncmp(mailbox, "user.", 5))
	return 0;

    /* see if the mailbox matches one of our wildmats */
    while (wild->pat && wildmat(mailbox, wild->pat) != 1) wild++;

    /* if we don't have a match, or its a negative match, skip it */
    if (!wild->pat || wild->not) return 0;

    /* we only care about shared /comment */
    if (userid[0]) return 0;

    prot_printf(nntp_out, "%s\t%s\r\n", mailbox+strlen(newsprefix),
		value->s);

    return 0;
}