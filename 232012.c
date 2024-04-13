int service_init(int argc __attribute__((unused)),
		 char **argv __attribute__((unused)),
		 char **envp __attribute__((unused)))
{
    int opt;
    const char *prefix;

    initialize_nntp_error_table();

    if (geteuid() == 0) fatal("must run as the Cyrus user", EC_USAGE);
    setproctitle_init(argc, argv, envp);

    /* set signal handlers */
    signals_set_shutdown(&shut_down);
    signal(SIGPIPE, SIG_IGN);

    /* load the SASL plugins */
    global_sasl_init(1, 1, mysasl_cb);

    if ((prefix = config_getstring(IMAPOPT_NEWSPREFIX)))
	snprintf(newsprefix, sizeof(newsprefix), "%s.", prefix);

    newsgroups = split_wildmats((char *) config_getstring(IMAPOPT_NEWSGROUPS));

    /* initialize duplicate delivery database */
    if (duplicate_init(NULL, 0) != 0) {
	syslog(LOG_ERR, 
	       "unable to init duplicate delivery database\n");
	fatal("unable to init duplicate delivery database", EC_SOFTWARE);
    }

    /* open the mboxlist, we'll need it for real work */
    mboxlist_init(0);
    mboxlist_open(NULL);

    /* open the quota db, we'll need it for expunge */
    quotadb_init(0);
    quotadb_open(NULL);

    /* open the user deny db */
    denydb_init(0);
    denydb_open(NULL);

    /* setup for sending IMAP IDLE notifications */
    idle_enabled();

    while ((opt = getopt(argc, argv, "srfp:")) != EOF) {
	switch(opt) {
	case 's': /* nntps (do starttls right away) */
	    nntps = 1;
	    if (!tls_enabled()) {
		syslog(LOG_ERR, "nntps: required OpenSSL options not present");
		fatal("nntps: required OpenSSL options not present",
		      EC_CONFIG);
	    }
	    break;

	case 'r': /* enter reader-only mode */
	    nntp_capa = MODE_READ;
	    break;

	case 'f': /* enter feeder-only mode */
	    nntp_capa = MODE_FEED;
	    break;

	case 'p': /* external protection */
	    extprops_ssf = atoi(optarg);
	    break;

	default:
	    usage();
	}
    }

    /* Initialize the annotatemore extention */
    annotatemore_init(NULL, NULL);
    annotatemore_open();

    newsmaster = (char *) config_getstring(IMAPOPT_NEWSMASTER);
    newsmaster_authstate = auth_newstate(newsmaster);

    singleinstance = config_getswitch(IMAPOPT_SINGLEINSTANCESTORE);

    /* Create a protgroup for input from the client and selected backend */
    protin = protgroup_new(2);

    return 0;
}