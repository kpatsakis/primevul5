static int parserange(char *str, uint32_t *uid, uint32_t *last,
		      char **msgid, struct backend **ret)
{
    const char *p = NULL;
    char *mboxname;
    int r = 0;

    *uid = 0;
    if (last) *last = 0;
    if (msgid) *msgid = NULL;
    if (ret) *ret = NULL;

    if (!str || !*str) {
	/* no argument, use current article */
	if (backend_current) {
	    if (ret) *ret = backend_current;
	}
	else if (!group_state) goto noopengroup;
	else if (!nntp_current) goto nocurrent;
	else {
	    *uid = index_getuid(group_state, nntp_current);
	    if (last) *last = *uid;
	}
    }
    else if (*str == '<') {
	/* message-id, find server and/or mailbox */
	if (!msgid) goto badrange;
	if (!my_find_msgid(str, &mboxname, uid)) goto nomsgid;

	*msgid = str;

	/* open group if its different from our current one */
	if (!group_state || strcmp(mboxname, group_state->mailbox->name)) {
	    if ((r = open_group(mboxname, 1, ret, NULL))) goto nomsgid;
	}
    }
    else if (backend_current)
	*ret = backend_current;
    else if (!group_state) goto noopengroup;
    else if (parseuint32(str, &p, uid) || uid == 0) goto badrange;
    else if (p && *p) {
	/* extra stuff, check for range */
	if (!last || (*p != '-')) goto badrange;
	if (*++p) {
	    if (parseuint32(p, NULL, last))
		*last = 0;
	}
	else
	    *last = UINT32_MAX;  /* open range -> use highest possible UID */
    }

    if (last && !*last) *last = *uid;

    return 0;

  noopengroup:
    prot_printf(nntp_out, "412 No newsgroup selected\r\n");
    return -1;

  nocurrent:
    prot_printf(nntp_out, "420 Current article number is invalid\r\n");
    return -1;

  nomsgid:
    prot_printf(nntp_out, "430 No article found with that message-id");
    if (r) prot_printf(nntp_out, " (%s)", error_message(r));
    prot_printf(nntp_out, "\r\n");
    return -1;

  badrange:
    prot_printf(nntp_out, "501 Bad message-id, message number, or range\r\n");
    return -1;
}