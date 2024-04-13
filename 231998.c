static int deliver(message_data_t *msg)
{
    int n, r = 0, myrights;
    char *rcpt = NULL;
    unsigned long uid;
    struct body *body = NULL;
    struct dest *dlist = NULL;
    duplicate_key_t dkey = {msg->id, NULL, msg->date};

    /* check ACLs of all mailboxes */
    for (n = 0; n < msg->rcpt.count; n++) {
	struct mboxlist_entry *mbentry = NULL;
	rcpt = msg->rcpt.data[n];
	dkey.to = rcpt;

	/* look it up */
	r = mlookup(rcpt, &mbentry);
	if (r) return IMAP_MAILBOX_NONEXISTENT;

	if (!(mbentry->acl && (myrights = cyrus_acl_myrights(nntp_authstate, mbentry->acl)) &&
	      (myrights & ACL_POST))) {
	    mboxlist_entry_free(&mbentry);
	    return IMAP_PERMISSION_DENIED;
	}

	if (mbentry->server) {
	    /* remote group */
	    proxy_adddest(&dlist, NULL, 0, mbentry->server, "");
	}
	else {
	    /* local group */
	    struct appendstate as;

	    if (msg->id && 
		duplicate_check(&dkey)) {
		/* duplicate message */
		duplicate_log(&dkey, "nntp delivery");
		continue;
	    }

	    r = append_setup(&as, rcpt, nntp_userid, nntp_authstate,
			     ACL_POST, 0, NULL, 0);

	    if (!r) {
		prot_rewind(msg->data);
		if (stage) {
		    r = append_fromstage(&as, &body, stage, 0,
					 NULL, !singleinstance,
					 /*annotations*/NULL);
		} else {
		    /* XXX should never get here */
		    r = append_fromstream(&as, &body, msg->data, msg->size, 0,
					  (const char **) NULL, 0);
		}
		if (r || ( msg->id && duplicate_check(&dkey) ) ) {    
		    append_abort(&as);
                   
		    if (!r) {
			/* duplicate message */
			duplicate_log(&dkey, "nntp delivery");
			continue;
		    }            
		}                
		else {           
		    r = append_commit(&as, NULL, &uid, NULL, NULL);
		}
	    }

	    if (!r && msg->id)
		duplicate_mark(&dkey, time(NULL), uid);

	    if (r) {
		mboxlist_entry_free(&mbentry);
		return r;
	    }
	}
	mboxlist_entry_free(&mbentry);
    }

    if (body) {
	message_free_body(body);
	free(body);
    }

    if (dlist) {
	struct dest *d;

	/* run the txns */
	r = deliver_remote(msg, dlist);

	/* free the destination list */
	d = dlist;
	while (d) {
	    struct dest *nextd = d->next;
	    free(d);
	    d = nextd;
	}
    }

    return r;
}