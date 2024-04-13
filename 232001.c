static int do_newsgroups(char *name, void *rock)
{
    struct list_rock *lrock = (struct list_rock *) rock;
    struct mboxlist_entry *mbentry = NULL;
    int r;

    r = mlookup(name, &mbentry);

    if (r || !mbentry->acl ||
	!(cyrus_acl_myrights(nntp_authstate, mbentry->acl) && ACL_LOOKUP)) {
	mboxlist_entry_free(&mbentry);
	return 0;
    }

    if (mbentry->server) {
	/* remote group */
	if (!hash_lookup(mbentry->server, &lrock->server_table)) {
	    /* add this server to our table */
	    hash_insert(mbentry->server, (void *)0xDEADBEEF, &lrock->server_table);
	}
	mboxlist_entry_free(&mbentry);
    }
    else {
	/* local group */
	mboxlist_entry_free(&mbentry);
	return CYRUSDB_DONE;
    }

    return 0;
}