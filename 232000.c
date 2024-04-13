static int do_active(char *name, void *rock)
{
    struct list_rock *lrock = (struct list_rock *) rock;
    int r, postable;
    struct backend *be;

    /* open the group */
    r = open_group(name, 1, &be, &postable);
    if (r) {
	/* can't open group, skip it */
    }
    else if (be) {
	if (!hash_lookup(be->hostname, &lrock->server_table)) {
	    /* add this server to our table */
	    hash_insert(be->hostname, (void *)0xDEADBEEF, &lrock->server_table);
	}
    }
    else {
	prot_printf(nntp_out, "%s %u %u %c\r\n", name+strlen(newsprefix),
		    group_state->exists ? index_getuid(group_state, group_state->exists) :
		    group_state->mailbox->i.last_uid,
		    group_state->exists ? index_getuid(group_state, 1) :
		    group_state->mailbox->i.last_uid+1,
		    postable ? 'y' : 'n');
	index_close(&group_state);
    }

    return 0;
}