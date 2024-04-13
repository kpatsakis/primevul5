main(int argc, char* argv[])
{
	int c;
	const char* cfgfile = CONFIGFILE;
	const char* winopt = NULL;
	const char* log_ident_default;
	int cmdline_verbose = 0;
	int debug_mode = 0;
	int need_pidfile = 1;

#ifdef UB_ON_WINDOWS
	int cmdline_cfg = 0;
#endif

	log_init(NULL, 0, NULL);
	log_ident_default = strrchr(argv[0],'/')?strrchr(argv[0],'/')+1:argv[0];
	log_ident_set_default(log_ident_default);
	log_ident_set(log_ident_default);
	/* parse the options */
	while( (c=getopt(argc, argv, "c:dhpvw:V")) != -1) {
		switch(c) {
		case 'c':
			cfgfile = optarg;
#ifdef UB_ON_WINDOWS
			cmdline_cfg = 1;
#endif
			break;
		case 'v':
			cmdline_verbose++;
			verbosity++;
			break;
		case 'p':
			need_pidfile = 0;
			break;
		case 'd':
			debug_mode++;
			break;
		case 'w':
			winopt = optarg;
			break;
		case 'V':
			print_build_options();
			return 0;
		case '?':
		case 'h':
		default:
			usage();
			return 1;
		}
	}
	argc -= optind;
	/* argv += optind; not using further arguments */

	if(winopt) {
#ifdef UB_ON_WINDOWS
		wsvc_command_option(winopt, cfgfile, cmdline_verbose, 
			cmdline_cfg);
#else
		fatal_exit("option not supported");
#endif
	}

	if(argc != 0) {
		usage();
		return 1;
	}

	run_daemon(cfgfile, cmdline_verbose, debug_mode, need_pidfile);
	log_init(NULL, 0, NULL); /* close logfile */
#ifndef unbound_testbound
	if(log_get_lock()) {
		lock_basic_destroy((lock_basic_type*)log_get_lock());
	}
#endif
	return 0;
}