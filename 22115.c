void controller::setup_dirs(bool silent) {
	const char * env_home;
	if (!(env_home = ::getenv("HOME"))) {
		struct passwd * spw = ::getpwuid(::getuid());
		if (spw) {
			env_home = spw->pw_dir;
		} else {
			std::cerr << _("Fatal error: couldn't determine home directory!") << std::endl;
			std::cerr << strprintf::fmt(_("Please set the HOME environment variable or add a valid user for UID %u!"), ::getuid()) << std::endl;
			::exit(EXIT_FAILURE);
		}
	}

	config_dir = env_home;
	config_dir.append(NEWSBEUTER_PATH_SEP);
	config_dir.append(NEWSBEUTER_CONFIG_SUBDIR);

	if (setup_dirs_xdg(env_home, silent))
		return;

	mkdir(config_dir.c_str(),0700); // create configuration directory if it doesn't exist

	url_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + url_file;
	cache_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + cache_file;
	lock_file = cache_file + LOCK_SUFFIX;
	config_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + config_file;
	queue_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + queue_file;

	searchfile = strprintf::fmt("%s%shistory.search", config_dir, NEWSBEUTER_PATH_SEP);
	cmdlinefile = strprintf::fmt("%s%shistory.cmdline", config_dir, NEWSBEUTER_PATH_SEP);
}