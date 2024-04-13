void controller::run(int argc, char * argv[]) {
	int c;

	::signal(SIGINT, ctrl_c_action);
	::signal(SIGPIPE, ignore_signal);
	::signal(SIGHUP, ctrl_c_action);
	::signal(SIGCHLD, omg_a_child_died);

	bool do_import = false, do_export = false, cachefile_given_on_cmdline = false, do_vacuum = false;
	std::string importfile;
	bool do_read_import = false, do_read_export = false;
	std::string readinfofile;
	unsigned int show_version = 0;

	bool silent = false;
	bool execute_cmds = false;

	static const char getopt_str[] = "i:erhqu:c:C:d:l:vVoxXI:E:";
	static const struct option longopts[] = {
		{"cache-file"      , required_argument, 0, 'c'},
		{"config-file"     , required_argument, 0, 'C'},
		{"execute"         , required_argument, 0, 'x'},
		{"export-to-file"  , required_argument, 0, 'E'},
		{"export-to-opml"  , no_argument      , 0, 'e'},
		{"help"            , no_argument      , 0, 'h'},
		{"import-from-file", required_argument, 0, 'I'},
		{"import-from-opml", required_argument, 0, 'i'},
		{"log-file"        , required_argument, 0, 'd'},
		{"log-level"       , required_argument, 0, 'l'},
		{"quiet"           , no_argument      , 0, 'q'},
		{"refresh-on-start", no_argument      , 0, 'r'},
		{"url-file"        , required_argument, 0, 'u'},
		{"vacuum"          , no_argument      , 0, 'X'},
		{"version"         , no_argument      , 0, 'v'},
		{0                 , 0                , 0,  0 }
	};

	/* First of all, let's check for options that imply silencing of the
	 * output: import, export, command execution and, well, quiet mode */
	while ((c = ::getopt_long(argc, argv, getopt_str, longopts, nullptr)) != -1) {
		if (strchr("iexq", c) != nullptr) {
			silent = true;
			break;
		}
	}

	setup_dirs(silent);

	/* Now that silencing's set up, let's rewind to the beginning of argv and
	 * process the options */
	optind = 1;

	while ((c = ::getopt_long(argc, argv, getopt_str, longopts, nullptr)) != -1) {
		switch (c) {
		case ':': /* fall-through */
		case '?': /* missing option */
			usage(argv[0]);
			break;
		case 'i':
			if (do_export)
				usage(argv[0]);
			do_import = true;
			importfile = optarg;
			break;
		case 'r':
			refresh_on_start = true;
			break;
		case 'e':
			if (do_import)
				usage(argv[0]);
			do_export = true;
			break;
		case 'h':
			usage(argv[0]);
			break;
		case 'u':
			url_file = optarg;
			break;
		case 'c':
			cache_file = optarg;
			lock_file = std::string(cache_file) + LOCK_SUFFIX;
			cachefile_given_on_cmdline = true;
			break;
		case 'C':
			config_file = optarg;
			break;
		case 'X':
			do_vacuum = true;
			break;
		case 'v':
		case 'V':
			show_version++;
			break;
		case 'x':
			execute_cmds = true;
			break;
		case 'q':
			break;
		case 'd':
			logger::getInstance().set_logfile(optarg);
			break;
		case 'l': {
			level l = static_cast<level>(atoi(optarg));
			if (l > level::NONE && l <= level::DEBUG) {
				logger::getInstance().set_loglevel(l);
			} else {
				std::cerr << strprintf::fmt(_("%s: %d: invalid loglevel value"), argv[0], l) << std::endl;
				::std::exit(EXIT_FAILURE);
			}
		}
		break;
		case 'I':
			if (do_read_export)
				usage(argv[0]);
			do_read_import = true;
			readinfofile = optarg;
			break;
		case 'E':
			if (do_read_import)
				usage(argv[0]);
			do_read_export = true;
			readinfofile = optarg;
			break;
		default:
			std::cout << strprintf::fmt(_("%s: unknown option - %c"), argv[0], static_cast<char>(c)) << std::endl;
			usage(argv[0]);
			break;
		}
	};


	if (show_version) {
		version_information(argv[0], show_version);
	}

	if (do_import) {
		LOG(level::INFO,"Importing OPML file from %s",importfile);
		urlcfg = new file_urlreader(url_file);
		urlcfg->reload();
		import_opml(importfile);
		return;
	}


	LOG(level::INFO, "nl_langinfo(CODESET): %s", nl_langinfo(CODESET));

	if (!do_export) {

		if (!silent)
			std::cout << strprintf::fmt(_("Starting %s %s..."), PROGRAM_NAME, PROGRAM_VERSION) << std::endl;

		pid_t pid;
		if (!utils::try_fs_lock(lock_file, pid)) {
			if (pid > 0) {
				LOG(level::ERROR,"an instance is already running: pid = %u",pid);
			} else {
				LOG(level::ERROR,"something went wrong with the lock: %s", strerror(errno));
			}
			if (!execute_cmds) {
				std::cout << strprintf::fmt(_("Error: an instance of %s is already running (PID: %u)"), PROGRAM_NAME, pid) << std::endl;
			}
			return;
		}
	}

	if (!silent)
		std::cout << _("Loading configuration...");
	std::cout.flush();

	cfg.register_commands(cfgparser);
	colorman.register_commands(cfgparser);

	keymap keys(KM_NEWSBEUTER);
	cfgparser.register_handler("bind-key",&keys);
	cfgparser.register_handler("unbind-key",&keys);
	cfgparser.register_handler("macro", &keys);

	cfgparser.register_handler("ignore-article",&ign);
	cfgparser.register_handler("always-download",&ign);
	cfgparser.register_handler("reset-unread-on-update",&ign);

	cfgparser.register_handler("define-filter",&filters);
	cfgparser.register_handler("highlight", &rxman);
	cfgparser.register_handler("highlight-article", &rxman);

	try {
		cfgparser.parse("/etc/" PROGRAM_NAME "/config");
		cfgparser.parse(config_file);
	} catch (const configexception& ex) {
		LOG(level::ERROR,"an exception occurred while parsing the configuration file: %s",ex.what());
		std::cout << ex.what() << std::endl;
		utils::remove_fs_lock(lock_file);
		return;
	}

	update_config();

	if (!silent)
		std::cout << _("done.") << std::endl;

	// create cache object
	std::string cachefilepath = cfg.get_configvalue("cache-file");
	if (cachefilepath.length() > 0 && !cachefile_given_on_cmdline) {
		cache_file = cachefilepath.c_str();

		// ok, we got another cache file path via the configuration
		// that means we need to remove the old lock file, assemble
		// the new lock file's name, and then try to lock it.
		utils::remove_fs_lock(lock_file);
		lock_file = std::string(cache_file) + LOCK_SUFFIX;

		pid_t pid;
		if (!utils::try_fs_lock(lock_file, pid)) {
			if (pid > 0) {
				LOG(level::ERROR,"an instance is already running: pid = %u",pid);
			} else {
				LOG(level::ERROR,"something went wrong with the lock: %s", strerror(errno));
			}
			std::cout << strprintf::fmt(_("Error: an instance of %s is already running (PID: %u)"), PROGRAM_NAME, pid) << std::endl;
			return;
		}
	}

	if (!silent) {
		std::cout << _("Opening cache...");
		std::cout.flush();
	}
	try {
		rsscache = new cache(cache_file,&cfg);
	} catch (const dbexception& e) {
		std::cerr << strprintf::fmt(_("Error: opening the cache file `%s' failed: %s"), cache_file, e.what()) << std::endl;
		utils::remove_fs_lock(lock_file);
		::exit(EXIT_FAILURE);
	}

	if (!silent) {
		std::cout << _("done.") << std::endl;
	}



	std::string type = cfg.get_configvalue("urls-source");
	if (type == "local") {
		urlcfg = new file_urlreader(url_file);
	} else if (type == "opml") {
		urlcfg = new opml_urlreader(&cfg);
	} else if (type == "oldreader") {
		api = new oldreader_api(&cfg);
		urlcfg = new oldreader_urlreader(&cfg, url_file, api);
	} else if (type == "ttrss") {
		api = new ttrss_api(&cfg);
		urlcfg = new ttrss_urlreader(url_file, api);
	} else if (type == "newsblur") {
		api = new newsblur_api(&cfg);
		urlcfg = new newsblur_urlreader(url_file, api);
	} else if (type == "feedhq") {
		api = new feedhq_api(&cfg);
		urlcfg = new feedhq_urlreader(&cfg, url_file, api);
	} else if (type == "ocnews") {
		api = new ocnews_api(&cfg);
		urlcfg = new ocnews_urlreader(url_file, api);
	} else {
		LOG(level::ERROR,"unknown urls-source `%s'", urlcfg->get_source());
	}

	if (!do_export && !silent) {
		std::cout << strprintf::fmt(_("Loading URLs from %s..."), urlcfg->get_source());
		std::cout.flush();
	}
	if (api) {
		if (!api->authenticate()) {
			std::cout << "Authentication failed." << std::endl;
			utils::remove_fs_lock(lock_file);
			return;
		}
	}
	urlcfg->reload();
	if (!do_export && !silent) {
		std::cout << _("done.") << std::endl;
	}

	if (urlcfg->get_urls().size() == 0) {
		LOG(level::ERROR,"no URLs configured.");
		std::string msg;
		if (type == "local") {
			msg = strprintf::fmt(_("Error: no URLs configured. Please fill the file %s with RSS feed URLs or import an OPML file."), url_file);
		} else if (type == "opml") {
			msg = strprintf::fmt(_("It looks like the OPML feed you subscribed contains no feeds. Please fill it with feeds, and try again."));
		} else if (type == "oldreader") {
			msg = strprintf::fmt(_("It looks like you haven't configured any feeds in your The Old Reader account. Please do so, and try again."));
		} else if (type == "ttrss") {
			msg = strprintf::fmt(_("It looks like you haven't configured any feeds in your Tiny Tiny RSS account. Please do so, and try again."));
		} else if (type == "newsblur") {
			msg = strprintf::fmt(_("It looks like you haven't configured any feeds in your NewsBlur account. Please do so, and try again."));
		} else {
			assert(0); // shouldn't happen
		}
		std::cout << msg << std::endl << std::endl;
		usage(argv[0]);
	}

	if (!do_export && !do_vacuum && !silent)
		std::cout << _("Loading articles from cache...");
	if (do_vacuum)
		std::cout << _("Opening cache...");
	std::cout.flush();


	if (do_vacuum) {
		std::cout << _("done.") << std::endl;
		std::cout << _("Cleaning up cache thoroughly...");
		std::cout.flush();
		rsscache->do_vacuum();
		std::cout << _("done.") << std::endl;
		utils::remove_fs_lock(lock_file);
		return;
	}

	unsigned int i=0;
	for (auto url : urlcfg->get_urls()) {
		try {
			bool ignore_disp = (cfg.get_configvalue("ignore-mode") == "display");
			std::shared_ptr<rss_feed> feed = rsscache->internalize_rssfeed(url, ignore_disp ? &ign : nullptr);
			feed->set_tags(urlcfg->get_tags(url));
			feed->set_order(i);
			std::lock_guard<std::mutex> feedslock(feeds_mutex);
			feeds.push_back(feed);
		} catch(const dbexception& e) {
			std::cout << _("Error while loading feeds from database: ") << e.what() << std::endl;
			utils::remove_fs_lock(lock_file);
			return;
		} catch(const std::string& str) {
			std::cout << strprintf::fmt(_("Error while loading feed '%s': %s"), url, str) << std::endl;
			utils::remove_fs_lock(lock_file);
			return;
		}
		i++;
	}

	std::vector<std::string> tags = urlcfg->get_alltags();

	if (!do_export && !silent)
		std::cout << _("done.") << std::endl;

	// if configured, we fill all query feeds with some data; no need to sort it, it will be refilled when actually opening it.
	if (cfg.get_configvalue_as_bool("prepopulate-query-feeds")) {
		std::cout << _("Prepopulating query feeds...");
		std::cout.flush();
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		for (auto feed : feeds) {
			if (feed->rssurl().substr(0,6) == "query:") {
				feed->update_items(get_all_feeds_unlocked());
			}
		}
		std::cout << _("done.") << std::endl;
	}

	sort_feeds();

	if (do_export) {
		export_opml();
		utils::remove_fs_lock(lock_file);
		return;
	}

	if (do_read_import) {
		LOG(level::INFO,"Importing read information file from %s",readinfofile);
		std::cout << _("Importing list of read articles...");
		std::cout.flush();
		import_read_information(readinfofile);
		std::cout << _("done.") << std::endl;
		return;
	}

	if (do_read_export) {
		LOG(level::INFO,"Exporting read information file to %s",readinfofile);
		std::cout << _("Exporting list of read articles...");
		std::cout.flush();
		export_read_information(readinfofile);
		std::cout << _("done.") << std::endl;
		return;
	}

	// hand over the important objects to the view
	v->set_config_container(&cfg);
	v->set_keymap(&keys);
	v->set_tags(tags);

	if (execute_cmds) {
		execute_commands(argv, optind);
		utils::remove_fs_lock(lock_file);
		return;
	}

	// if the user wants to refresh on startup via configuration file, then do so,
	// but only if -r hasn't been supplied.
	if (!refresh_on_start && cfg.get_configvalue_as_bool("refresh-on-startup")) {
		refresh_on_start = true;
	}

	formaction::load_histories(searchfile, cmdlinefile);

	// run the view
	v->run();

	unsigned int history_limit = cfg.get_configvalue_as_int("history-limit");
	LOG(level::DEBUG, "controller::run: history-limit = %u", history_limit);
	formaction::save_histories(searchfile, cmdlinefile, history_limit);

	if (!silent) {
		std::cout << _("Cleaning up cache...");
		std::cout.flush();
	}
	try {
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		rsscache->cleanup_cache(feeds);
		if (!silent) {
			std::cout << _("done.") << std::endl;
		}
	} catch (const dbexception& e) {
		LOG(level::USERERROR, "Cleaning up cache failed: %s", e.what());
		if (!silent) {
			std::cout << _("failed: ") << e.what() << std::endl;
		}
	}

	utils::remove_fs_lock(lock_file);
}