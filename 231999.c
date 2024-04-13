static void cmd_authinfo_pass(char *pass)
{
    int failedloginpause;
    /* Conceal password in telemetry log */
    if (nntp_logfd != -1 && pass) {
	(void)ftruncate(nntp_logfd,
		  lseek(nntp_logfd, -2, SEEK_CUR) - strlen(pass));
	(void)write(nntp_logfd, "...\r\n", 5);
    }

    if (nntp_authstate) {
	prot_printf(nntp_out, "502 Already authenticated\r\n");
	return;
    }

    if (!nntp_userid) {
	prot_printf(nntp_out, "482 Must give AUTHINFO USER command first\r\n");
	return;
    }

    if (!strcmp(nntp_userid, "anonymous")) {
	if (allowanonymous) {
	    pass = beautify_string(pass);
	    if (strlen(pass) > 500) pass[500] = '\0';
	    syslog(LOG_NOTICE, "login: %s anonymous %s",
		   nntp_clienthost, pass);
	}
	else {
	    syslog(LOG_NOTICE, "badlogin: %s anonymous login refused",
		   nntp_clienthost);
	    prot_printf(nntp_out, "481 Invalid login\r\n");
	    return;
	}
    }
    else if (sasl_checkpass(nntp_saslconn,
			    nntp_userid,
			    strlen(nntp_userid),
			    pass,
			    strlen(pass))!=SASL_OK) { 
	syslog(LOG_NOTICE, "badlogin: %s plaintext %s %s",
	       nntp_clienthost, nntp_userid, sasl_errdetail(nntp_saslconn));
	failedloginpause = config_getint(IMAPOPT_FAILEDLOGINPAUSE);
	if (failedloginpause != 0) {
	    sleep(failedloginpause);
	}
	prot_printf(nntp_out, "481 Invalid login\r\n");
	free(nntp_userid);
	nntp_userid = 0;

	return;
    }
    else {
	syslog(LOG_NOTICE, "login: %s %s plaintext%s %s", nntp_clienthost,
	       nntp_userid, nntp_starttls_done ? "+TLS" : "",
	       "User logged in");

	prot_printf(nntp_out, "281 User logged in\r\n");

	nntp_authstate = auth_newstate(nntp_userid);

	/* Close IP-based telemetry log and create new log based on userid */
	if (nntp_logfd != -1) close(nntp_logfd);
	nntp_logfd = telemetry_log(nntp_userid, nntp_in, nntp_out, 0);
    }
}