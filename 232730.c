apply_settings(struct daemon* daemon, struct config_file* cfg,
	int cmdline_verbose, int debug_mode)
{
	/* apply if they have changed */
	verbosity = cmdline_verbose + cfg->verbosity;
	if (debug_mode > 1) {
		cfg->use_syslog = 0;
		free(cfg->logfile);
		cfg->logfile = NULL;
	}
	daemon_apply_cfg(daemon, cfg);
	checkrlimits(cfg);

	if (cfg->use_systemd && cfg->do_daemonize) {
		log_warn("use-systemd and do-daemonize should not be enabled at the same time");
	}

	log_ident_set_or_default(cfg->log_identity);
}