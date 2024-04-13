static void cmd_authinfo_user(char *user)
{
    const char *p;

    if (nntp_authstate) {
	prot_printf(nntp_out, "502 Already authenticated\r\n");
	return;
    }

    /* possibly disallow USER */
    if (!(nntp_starttls_done || (extprops_ssf > 1) ||
	  config_getswitch(IMAPOPT_ALLOWPLAINTEXT))) {
	prot_printf(nntp_out,
		    "483 AUTHINFO USER command only available under a layer\r\n");
	return;
    }

    if (nntp_userid) {
	free(nntp_userid);
	nntp_userid = NULL;
    }

    if (!(p = canonify_userid(user, NULL, NULL))) {
	prot_printf(nntp_out, "481 Invalid user\r\n");
	syslog(LOG_NOTICE,
	       "badlogin: %s plaintext %s invalid user",
	       nntp_clienthost, beautify_string(user));
    }
    else {
	nntp_userid = xstrdup(p);
	prot_printf(nntp_out, "381 Give AUTHINFO PASS command\r\n");
    }
}