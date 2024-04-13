static int savemsg(message_data_t *m, FILE *f)
{
    struct stat sbuf;
    const char **body, **groups;
    int r, i;
    time_t now = time(NULL);
    static int post_count = 0;
    FILE *stagef = NULL;
    const char *skipheaders[] = {
	"Path",		/* need to prepend our servername */
	"Xref",		/* need to remove (generated on the fly) */
	"Reply-To",	/* need to add "post" email addresses */
	NULL
    };

    m->f = f;

    /* fill the cache */
    r = spool_fill_hdrcache(nntp_in, f, m->hdrcache, skipheaders);
    if (r) {
	/* got a bad header */

	/* flush the remaining output */
	spool_copy_msg(nntp_in, NULL);
	return r;
    }

    /* now, using our header cache, fill in the data that we want */

    /* get path */
    if ((body = spool_getheader(m->hdrcache, "path")) != NULL) {
	/* prepend to the cached path */
	m->path = strconcat(config_servername, "!", body[0], (char *)NULL);
	spool_replace_header(xstrdup("Path"), xstrdup(m->path), m->hdrcache);
    } else {
	/* no path, create one */
	m->path = strconcat(config_servername, "!",
			    nntp_userid ? nntp_userid : "anonymous",
			    (char *)NULL);
	spool_cache_header(xstrdup("Path"), xstrdup(m->path), m->hdrcache);
    }
    fprintf(f, "Path: %s\r\n", m->path);

    /* get message-id */
    if ((body = spool_getheader(m->hdrcache, "message-id")) != NULL) {
	m->id = xstrdup(body[0]);
    } else {
	/* no message-id, create one */
	pid_t p = getpid();

	m->id = xmalloc(40 + strlen(config_servername));
	sprintf(m->id, "<cmu-nntpd-%d-%d-%d@%s>", p, (int) now, 
		post_count++, config_servername);
	fprintf(f, "Message-ID: %s\r\n", m->id);
	spool_cache_header(xstrdup("Message-ID"), xstrdup(m->id), m->hdrcache);
    }

    /* get date */
    if ((body = spool_getheader(m->hdrcache, "date")) == NULL) {
	/* no date, create one */
	char datestr[RFC822_DATETIME_MAX+1];

	time_to_rfc822(now, datestr, sizeof(datestr));
	m->date = xstrdup(datestr);
	fprintf(f, "Date: %s\r\n", datestr);
	spool_cache_header(xstrdup("Date"), xstrdup(datestr), m->hdrcache);
    }
    else {
	m->date = xstrdup(body[0]);
    }

    /* get control */
    if ((body = spool_getheader(m->hdrcache, "control")) != NULL) {
	size_t len;
	char *s;

	m->control = xstrdup(body[0]);

	/* create a recipient for the appropriate pseudo newsgroup */
	len = strcspn(m->control, " \t\r\n");
	s = xmalloc(strlen(newsprefix) + 8 + len + 1);
	sprintf(s, "%scontrol.%.*s", newsprefix, (int) len, m->control);

	strarray_appendm(&m->rcpt, s);
    } else {
	m->control = NULL;	/* no control */

	/* get newsgroups */
	if ((groups = spool_getheader(m->hdrcache, "newsgroups")) != NULL) {
	    /* parse newsgroups and create recipients */
	    parse_groups(groups[0], m);
	    if (!m->rcpt.count) {
		r = IMAP_MAILBOX_NONEXISTENT; /* no newsgroups that we serve */
	    }
	    if (!r) {
		const char *newspostuser = config_getstring(IMAPOPT_NEWSPOSTUSER);
		/* get reply-to */
		body = spool_getheader(m->hdrcache, "reply-to");

		/* add Reply-To: header */
		if (body || newspostuser) {
		    const char **postto, *p;
		    int fold = 0;
		    const char *sep = "";
		    char *replyto;
		    size_t n;
		    struct buf buf = BUF_INITIALIZER;

		    if (newspostuser) {
			/* add "post" email addresses based on newsgroup */

			/* determine which groups header to use */
			postto = spool_getheader(m->hdrcache, "followup-to");
			if (!postto) postto = groups;

			if (body) {
			    /* append to the cached header */
			    buf_appendcstr(&buf, body[0]);
			    fold = buf.len+1;
			    sep = ", ";
			}
			for (p = postto[0];; p += n) {
			    /* skip whitespace */
			    while (p && *p &&
				   (Uisspace(*p) || *p == ',')) p++;
			    if (!p || !*p) break;

			    /* find end of group name */
			    n = strcspn(p, ", \t");

			    /* add the post address */
			    buf_printf(&buf, "%s%s+%.*s",
					 sep, newspostuser, (int) n, p);

			    sep = ", ";
			}
			replyto = buf_release(&buf);
			if (body) {
			    /* replace the existing header */
			    spool_replace_header(xstrdup("Reply-To"), replyto,
					         m->hdrcache);
			} else {
			    /* add the new header to the cache */
			    spool_cache_header(xstrdup("Reply-To"), replyto,
					       m->hdrcache);
			}
		    } else {
			/* no newspostuser, use original replyto */
			replyto = (char *) body[0];
		    }

		    /* add the header to the file */
		    fprintf(f, "Reply-To: ");
		    if (fold)
			fprintf(f, "%.*s\r\n\t", fold, replyto);
		    fprintf(f, "%s\r\n", replyto+fold);
		}
	    }
	} else {
	    r = NNTP_NO_NEWSGROUPS;		/* no newsgroups header */
	}

	if (r) {
	    /* error getting newsgroups */

	    /* flush the remaining output */
	    spool_copy_msg(nntp_in, NULL);
	    return r;
	}
    }

    fflush(f);
    if (ferror(f)) {
	return IMAP_IOERROR;
    }

    if (fstat(fileno(f), &sbuf) == -1) {
	return IMAP_IOERROR;
    }

    /* spool to the stage of one of the recipients */
    for (i = 0; !stagef && (i < m->rcpt.count); i++) {
	stagef = append_newstage(m->rcpt.data[i], now, 0, &stage);
    }

    if (stagef) {
	const char *base = 0;
	unsigned long size = 0;
	int n;

	/* copy the header from our tmpfile to the stage */
	map_refresh(fileno(f), 1, &base, &size, sbuf.st_size, "tmp", 0);
	n = retry_write(fileno(stagef), base, size);
	map_free(&base, &size);

	if (n == -1) {
	    /* close and remove the stage */
	    fclose(stagef);
	    append_removestage(stage);
	    stage = NULL;
	    return IMAP_IOERROR;
	}
	else {
	    /* close the tmpfile and use the stage */
	    fclose(f);
	    m->f = f = stagef;
	}
    }
    /* else this is probably a remote group, so use the tmpfile */

    r = spool_copy_msg(nntp_in, f);

    if (r) return r;

    fflush(f);
    if (ferror(f)) {
	return IMAP_IOERROR;
    }

    if (fstat(fileno(f), &sbuf) == -1) {
	return IMAP_IOERROR;
    }
    m->size = sbuf.st_size;
    m->data = prot_new(fileno(f), 0);

    return 0;
}