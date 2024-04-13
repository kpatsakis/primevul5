static void feedpeer(char *peer, message_data_t *msg)
{
    char *user, *pass, *host, *port, *wild, *path, *s;
    int oldform = 0;
    struct wildmat *wmat = NULL, *w;
    int len, err, n, feed = 1;
    struct addrinfo hints, *res, *res0;
    int sock = -1;
    struct protstream *pin, *pout;
    char buf[4096];
    int body = 0, skip;

    /* parse the peer */
    user = pass = host = port = wild = NULL;
    if ((wild = strrchr(peer, '/')))
	*wild++ = '\0';
    else if ((wild = strrchr(peer, ':')) &&
	     strcspn(wild, "!*?,.") != strlen(wild)) {
	*wild++ = '\0';
	host = peer;
	oldform = 1;
    }
    if (!oldform) {
	if ((host = strchr(peer, '@'))) {
	    *host++ = '\0';
	    user = peer;
	    if ((pass = strchr(user, ':'))) *pass++ = '\0';
	}
	else
	    host = peer;

	if ((port = strchr(host, ':'))) *port++ = '\0';
    }

    /* check path to see if this message came through our peer */
    len = strlen(host);
    path = msg->path;
    while (path && (s = strchr(path, '!'))) {
	if ((s - path) == len && !strncmp(path, host, len)) {
	    return;
	}
	path = s + 1;
    }

    /* check newsgroups against wildmat to see if we should feed it */
    if (wild && *wild) {
	wmat = split_wildmats(wild);

	feed = 0;
	for (n = 0; n < msg->rcpt.count; n++) {
	    /* see if the newsgroup matches one of our wildmats */
	    w = wmat;
	    while (w->pat &&
		   wildmat(msg->rcpt.data[n], w->pat) != 1) {
		w++;
	    }

	    if (w->pat) {
		/* we have a match, check to see what kind of match */
		if (!w->not) {
		    /* positive match, ok to feed, keep checking */
		    feed = 1;
		}
		else if (w->not < 0) {
		    /* absolute negative match, do not feed */
		    feed = 0;
		    break;
		}
		else {
		    /* negative match, keep checking */
		}
	    }
	    else {
		/* no match, keep checking */
	    }
	}

	free_wildmats(wmat);
    }

    if (!feed) return;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    if (!port || !*port) port = "119";
    if ((err = getaddrinfo(host, port, &hints, &res0)) != 0) {
	syslog(LOG_ERR, "getaddrinfo(%s, %s) failed: %m", host, port);
	return;
    }

    for (res = res0; res; res = res->ai_next) {
	if ((sock = socket(res->ai_family, res->ai_socktype,
			   res->ai_protocol)) < 0)
	    continue;
	if (connect(sock, res->ai_addr, res->ai_addrlen) >= 0)
	    break;
	close(sock);
	sock = -1;
    }
    freeaddrinfo(res0);
    if(sock < 0) {
	syslog(LOG_ERR, "connect(%s:%s) failed: %m", host, port);
	return;
    }
    
    pin = prot_new(sock, 0);
    pout = prot_new(sock, 1);
    prot_setflushonread(pin, pout);

    /* read the initial greeting */
    if (!prot_fgets(buf, sizeof(buf), pin) || strncmp("200", buf, 3)) {
	syslog(LOG_ERR, "peer doesn't allow posting");
	goto quit;
    }

    if (user) {
	/* change to reader mode - not always necessary, so ignore result */
	prot_printf(pout, "MODE READER\r\n");
	prot_fgets(buf, sizeof(buf), pin);

	if (*user) {
	    /* authenticate to peer */
	    /* XXX this should be modified to support SASL and STARTTLS */

	    prot_printf(pout, "AUTHINFO USER %s\r\n", user);
	    if (!prot_fgets(buf, sizeof(buf), pin)) {
		syslog(LOG_ERR, "AUTHINFO USER terminated abnormally");
		goto quit;
	    }
	    else if (!strncmp("381", buf, 3)) {
		/* password required */
		if (!pass) {
		    syslog(LOG_ERR, "need password for AUTHINFO PASS");
		    goto quit;
		}

		prot_printf(pout, "AUTHINFO PASS %s\r\n", pass);
		if (!prot_fgets(buf, sizeof(buf), pin)) {
		    syslog(LOG_ERR, "AUTHINFO PASS terminated abnormally");
		    goto quit;
		}
	    }

	    if (strncmp("281", buf, 3)) {
		/* auth failed */
		syslog(LOG_ERR, "authentication failed");
		goto quit;
	    }
	}

	/* tell the peer we want to post */
	prot_printf(pout, "POST\r\n");
	prot_flush(pout);

	if (!prot_fgets(buf, sizeof(buf), pin) || strncmp("340", buf, 3)) {
	    syslog(LOG_ERR, "peer doesn't allow posting");
	    goto quit;
	}
    }
    else {
	/* tell the peer about our new article */
	prot_printf(pout, "IHAVE %s\r\n", msg->id);
	prot_flush(pout);

	if (!prot_fgets(buf, sizeof(buf), pin) || strncmp("335", buf, 3)) {
	    syslog(LOG_ERR, "peer doesn't want article %s", msg->id);
	    goto quit;
	}
    }

    /* send the article */
    rewind(msg->f);
    while (fgets(buf, sizeof(buf), msg->f)) {
	if (!body && buf[0] == '\r' && buf[1] == '\n') {
	    /* blank line between header and body */
	    body = 1;
	}

	skip = 0;
	if (!body) {
	    if (!strncasecmp(buf, "Reply-To:", 9)) {
		/* strip any post addresses, skip if becomes empty */
		if (!strip_post_addresses(buf+9)) skip = 1;
	    }
	}

	if (!skip && buf[0] == '.') prot_putc('.', pout);
	do {
	    if (!skip) prot_printf(pout, "%s", buf);
	} while (buf[strlen(buf)-1] != '\n' &&
		 fgets(buf, sizeof(buf), msg->f));
    }

    /* Protect against messages not ending in CRLF */
    if (buf[strlen(buf)-1] != '\n') prot_printf(pout, "\r\n");

    prot_printf(pout, ".\r\n");

    if (!prot_fgets(buf, sizeof(buf), pin) || strncmp("2", buf, 1)) {
	syslog(LOG_ERR, "article %s transfer to peer failed", msg->id);
    }

  quit:
    prot_printf(pout, "QUIT\r\n");
    prot_flush(pout);

    prot_fgets(buf, sizeof(buf), pin);

    /* Flush the incoming buffer */
    prot_NONBLOCK(pin);
    prot_fill(pin);

    /* close/free socket & prot layer */
    close(sock);
    
    prot_free(pin);
    prot_free(pout);

    return;
}