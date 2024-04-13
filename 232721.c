run_daemon(const char* cfgfile, int cmdline_verbose, int debug_mode, int need_pidfile)
{
	struct config_file* cfg = NULL;
	struct daemon* daemon = NULL;
	int done_setup = 0;

	if(!(daemon = daemon_init()))
		fatal_exit("alloc failure");
	while(!daemon->need_to_exit) {
		if(done_setup)
			verbose(VERB_OPS, "Restart of %s.", PACKAGE_STRING);
		else	verbose(VERB_OPS, "Start of %s.", PACKAGE_STRING);

		/* config stuff */
		if(!(cfg = config_create()))
			fatal_exit("Could not alloc config defaults");
		if(!config_read(cfg, cfgfile, daemon->chroot)) {
			if(errno != ENOENT)
				fatal_exit("Could not read config file: %s."
					" Maybe try unbound -dd, it stays on "
					"the commandline to see more errors, "
					"or unbound-checkconf", cfgfile);
			log_warn("Continuing with default config settings");
		}
		apply_settings(daemon, cfg, cmdline_verbose, debug_mode);
		if(!done_setup)
			config_lookup_uid(cfg);
	
		/* prepare */
		if(!daemon_open_shared_ports(daemon))
			fatal_exit("could not open ports");
		if(!done_setup) { 
			perform_setup(daemon, cfg, debug_mode, &cfgfile, need_pidfile);
			done_setup = 1; 
		} else {
			/* reopen log after HUP to facilitate log rotation */
			if(!cfg->use_syslog)
				log_init(cfg->logfile, 0, cfg->chrootdir);
		}
		/* work */
		daemon_fork(daemon);

		/* clean up for restart */
		verbose(VERB_ALGO, "cleanup.");
		daemon_cleanup(daemon);
		config_delete(cfg);
	}
	verbose(VERB_ALGO, "Exit cleanup.");
	/* this unlink may not work if the pidfile is located outside
	 * of the chroot/workdir or we no longer have permissions */
	if(daemon->pidfile) {
		int fd;
		/* truncate pidfile */
		fd = open(daemon->pidfile, O_WRONLY | O_TRUNC, 0644);
		if(fd != -1)
			close(fd);
		/* delete pidfile */
		unlink(daemon->pidfile);
	}
	daemon_delete(daemon);
}