static void cmd_authinfo_sasl(char *cmd, char *mech, char *resp)
{
    int r, sasl_result;
    char *success_data;
    sasl_ssf_t ssf;
    char *ssfmsg = NULL;
    const void *val;
    int failedloginpause;
    struct proc_limits limits;

    /* Conceal initial response in telemetry log */
    if (nntp_logfd != -1 && resp) {
	(void)ftruncate(nntp_logfd,
		  lseek(nntp_logfd, -2, SEEK_CUR) - strlen(resp));
	(void)write(nntp_logfd, "...\r\n", 5);
    }

    if (nntp_userid) {
	prot_printf(nntp_out, "502 Already authenticated\r\n");
	return;
    }

    /* Stop telemetry logging during SASL exchange */
    if (nntp_logfd != -1 && mech) {
	prot_setlog(nntp_in, PROT_NO_FD);
	prot_setlog(nntp_out, PROT_NO_FD);
    }

    if (cmd[0] == 'g') {
	/* AUTHINFO GENERIC */
	if (!mech) {
	    /* If client didn't specify any mech we give them the list */
	    const char *sasllist;
	    int mechnum;

	    prot_printf(nntp_out, "281 List of mechanisms follows\r\n");
      
	    /* CRLF separated, dot terminated */
	    if (sasl_listmech(nntp_saslconn, NULL,
			      "", "\r\n", "\r\n",
			      &sasllist,
			      NULL, &mechnum) == SASL_OK) {
		if (mechnum > 0) {
		    prot_printf(nntp_out, "%s", sasllist);
		}
	    }
      
	    prot_printf(nntp_out, ".\r\n");
	    return;
	}

	r = saslserver(nntp_saslconn, mech, resp, "AUTHINFO GENERIC ", "381 ",
		       "", nntp_in, nntp_out, &sasl_result, &success_data);
    }
    else {
	/* AUTHINFO SASL */
	r = saslserver(nntp_saslconn, mech, resp, "", "383 ", "=",
		       nntp_in, nntp_out, &sasl_result, &success_data);
    }

    /* Restart any telemetry logging */
    prot_setlog(nntp_in, nntp_logfd);
    prot_setlog(nntp_out, nntp_logfd);

    if (r) {
	int code;
	const char *errorstring = NULL;

	switch (r) {
	case IMAP_SASL_CANCEL:
	    prot_printf(nntp_out,
			"481 Client canceled authentication\r\n");
	    break;
	case IMAP_SASL_PROTERR:
	    errorstring = prot_error(nntp_in);

	    prot_printf(nntp_out,
			"482 Error reading client response: %s\r\n",
			errorstring ? errorstring : "");
	    break;
	default: 
	    /* failed authentication */
	    switch (sasl_result) {
	    case SASL_NOMECH:
	    case SASL_TOOWEAK:
		code = 503;
		break;
	    case SASL_ENCRYPT:
		code = 483;
		break;
	    case SASL_BADPROT:
		code = 482;
		break;
	    default:
		code = 481;
	    }

	    syslog(LOG_NOTICE, "badlogin: %s %s [%s]",
		   nntp_clienthost, mech, sasl_errdetail(nntp_saslconn));

	    failedloginpause = config_getint(IMAPOPT_FAILEDLOGINPAUSE);
	    if (failedloginpause != 0) {
	        sleep(failedloginpause);
	    }

	    /* Don't allow user probing */
	    if (sasl_result == SASL_NOUSER) sasl_result = SASL_BADAUTH;

	    errorstring = sasl_errstring(sasl_result, NULL, NULL);
	    if (errorstring) {
		prot_printf(nntp_out, "%d %s\r\n", code, errorstring);
	    } else {
		prot_printf(nntp_out, "%d Error authenticating\r\n", code);
	    }
	}

	reset_saslconn(&nntp_saslconn);
	return;
    }

    /* successful authentication */

    /* get the userid from SASL --- already canonicalized from
     * mysasl_proxy_policy()
     */
    sasl_result = sasl_getprop(nntp_saslconn, SASL_USERNAME, &val);
    if (sasl_result != SASL_OK) {
	prot_printf(nntp_out, "481 weird SASL error %d SASL_USERNAME\r\n", 
		    sasl_result);
	syslog(LOG_ERR, "weird SASL error %d getting SASL_USERNAME", 
	       sasl_result);
	reset_saslconn(&nntp_saslconn);
	return;
    }
    nntp_userid = xstrdup((const char *) val);

    sasl_getprop(nntp_saslconn, SASL_SSF, &val);
    ssf = *((sasl_ssf_t *) val);

    /* really, we should be doing a sasl_getprop on SASL_SSF_EXTERNAL,
       but the current libsasl doesn't allow that. */
    if (nntp_starttls_done) {
	switch(ssf) {
	case 0: ssfmsg = "tls protection"; break;
	case 1: ssfmsg = "tls plus integrity protection"; break;
	default: ssfmsg = "tls plus privacy protection"; break;
	}
    } else {
	switch(ssf) {
	case 0: ssfmsg = "no protection"; break;
	case 1: ssfmsg = "integrity protection"; break;
	default: ssfmsg = "privacy protection"; break;
	}
    }

    limits.procname = "nntpd";
    limits.clienthost = nntp_clienthost;
    limits.userid = nntp_userid;
    if (proc_checklimits(&limits)) {
	const char *sep = "";
	prot_printf(nntp_out,
		    "452 Too many open connections (");
	if (limits.maxhost) {
	    prot_printf(nntp_out, "%s%d of %d from %s", sep,
			limits.host, limits.maxhost, nntp_clienthost);
	    sep = ", ";
	}
	if (limits.maxuser) {
	    prot_printf(nntp_out, "%s%d of %d for %s", sep,
			limits.user, limits.maxuser, nntp_userid);
	}
	prot_printf(nntp_out, ")\r\n");
	reset_saslconn(&nntp_saslconn);
	free(nntp_userid);
	nntp_userid = NULL;
	return;
    }

    syslog(LOG_NOTICE, "login: %s %s %s%s %s", nntp_clienthost, nntp_userid,
	   mech, nntp_starttls_done ? "+TLS" : "", "User logged in");

    proc_register("nntpd", nntp_clienthost, nntp_userid, NULL);

    if (success_data) {
	prot_printf(nntp_out, "283 %s\r\n", success_data);
	free(success_data);
    } else {
	prot_printf(nntp_out, "281 Success (%s)\r\n", ssfmsg);
    }

    prot_setsasl(nntp_in,  nntp_saslconn);
    prot_setsasl(nntp_out, nntp_saslconn);

    /* Close IP-based telemetry log and create new log based on userid */
    if (nntp_logfd != -1) close(nntp_logfd);
    nntp_logfd = telemetry_log(nntp_userid, nntp_in, nntp_out, 0);

    if (ssf) {
	/* close any selected group */
	if (group_state)
	    index_close(&group_state);
	if (backend_current) {
	    proxy_downserver(backend_current);
	    backend_current = NULL;
	}
    }
}