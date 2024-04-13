bool controller::setup_dirs_xdg(const char *env_home, bool silent) {
	const char *env_xdg_config;
	const char *env_xdg_data;
	std::string xdg_config_dir;
	std::string xdg_data_dir;

	env_xdg_config = ::getenv("XDG_CONFIG_HOME");
	if (env_xdg_config) {
		xdg_config_dir = env_xdg_config;
	} else {
		xdg_config_dir = env_home;
		xdg_config_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_config_dir.append(".config");
	}

	env_xdg_data = ::getenv("XDG_DATA_HOME");
	if (env_xdg_data) {
		xdg_data_dir = env_xdg_data;
	} else {
		xdg_data_dir = env_home;
		xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_data_dir.append(".local");
		xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_data_dir.append("share");
	}

	xdg_config_dir.append(NEWSBEUTER_PATH_SEP);
	xdg_config_dir.append(NEWSBEUTER_SUBDIR_XDG);

	xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
	xdg_data_dir.append(NEWSBEUTER_SUBDIR_XDG);

	bool config_dir_exists = 0 == access(xdg_config_dir.c_str(), R_OK | X_OK);

	if (!config_dir_exists) {
		if (!silent) {
			std::cerr
				<< strprintf::fmt(
					   _("XDG: configuration directory '%s' not accessible, "
						 "using '%s' instead."),
					   xdg_config_dir,
					   config_dir)
				<< std::endl;
		}

		return false;
	}

	/* Invariant: config dir exists.
	 *
	 * At this point, we're confident we'll be using XDG. We don't check if
	 * data dir exists, because if it doesn't we'll create it. */

	config_dir = xdg_config_dir;

	// create data directory if it doesn't exist
	utils::mkdir_parents(xdg_data_dir, 0700);

	/* in config */
	url_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + url_file;
	config_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + config_file;

	/* in data */
	cache_file = xdg_data_dir + std::string(NEWSBEUTER_PATH_SEP) + cache_file;
	lock_file = cache_file + LOCK_SUFFIX;
	queue_file = xdg_data_dir + std::string(NEWSBEUTER_PATH_SEP) + queue_file;
	searchfile = strprintf::fmt("%s%shistory.search", xdg_data_dir, NEWSBEUTER_PATH_SEP);
	cmdlinefile = strprintf::fmt("%s%shistory.cmdline", xdg_data_dir, NEWSBEUTER_PATH_SEP);

	return true;
}