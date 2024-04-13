static int open_group(char *name, int has_prefix, struct backend **ret,
		      int *postable /* used for LIST ACTIVE only */)
{
    char mailboxname[MAX_MAILBOX_BUFFER];
    int r = 0;
    struct mboxlist_entry *mbentry = NULL;
    struct backend *backend_next = NULL;

    /* close local group */
    if (group_state) 
	index_close(&group_state);

    if (!has_prefix) {
	snprintf(mailboxname, sizeof(mailboxname), "%s%s", newsprefix, name);
	name = mailboxname;
    }

    if (!r) r = mlookup(name, &mbentry);

    if (!r && mbentry->acl) {
	int myrights = cyrus_acl_myrights(nntp_authstate, mbentry->acl);

	if (postable) *postable = myrights & ACL_POST;
	if (!postable && /* allow limited 'r' for LIST ACTIVE */
	    !(myrights & ACL_READ)) {
	    r = (myrights & ACL_LOOKUP) ?
		IMAP_PERMISSION_DENIED : IMAP_MAILBOX_NONEXISTENT;
	}
    }

    if (r) {
	mboxlist_entry_free(&mbentry);
	return r;
    }

    if (mbentry->server) {
	/* remote group */
	backend_next = proxy_findserver(mbentry->server, &nntp_protocol,
					nntp_userid ? nntp_userid : "anonymous",
					&backend_cached, &backend_current,
					NULL, nntp_in);
	mboxlist_entry_free(&mbentry);
	if (!backend_next) return IMAP_SERVER_UNAVAILABLE;

	*ret = backend_next;
    }
    else {
	/* local group */
	struct index_init init;

	mboxlist_entry_free(&mbentry);
	memset(&init, 0, sizeof(struct index_init));
	init.userid = nntp_userid;
	init.authstate = nntp_authstate;
	r = index_open(name, &init, &group_state);
	if (r) return r;

	if (ret) *ret = NULL;
    }

    syslog(LOG_DEBUG, "open: user %s opened %s",
	   nntp_userid ? nntp_userid : "anonymous", name);

    return 0;
}