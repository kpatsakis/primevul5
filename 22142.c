void controller::version_information(const char * argv0, unsigned int level) {
	if (level<=1) {
		std::cout << PROGRAM_NAME << " " << PROGRAM_VERSION << " - " << PROGRAM_URL << std::endl;
		std::cout << "Copyright (C) 2006-2015 Andreas Krennmair" << std::endl;
		std::cout << "Copyright (C) 2015-2017 Alexander Batischev" << std::endl;
		std::cout << "Copyright (C) 2006-2017 Newsbeuter contributors" << std::endl;
		std::cout << std::endl;

		std::cout << _("newsbeuter is free software and licensed under the MIT/X Consortium License.") << std::endl;
		std::cout << strprintf::fmt(_("Type `%s -vv' for more information."), argv0) << std::endl << std::endl;

		struct utsname xuts;
		uname(&xuts);
		std::cout << PROGRAM_NAME << " " << PROGRAM_VERSION << std::endl;
		std::cout << "System: " << xuts.sysname << " " << xuts.release << " (" << xuts.machine << ")" << std::endl;
#if defined(__GNUC__) && defined(__VERSION__)
		std::cout << "Compiler: g++ " << __VERSION__ << std::endl;
#endif
		std::cout << "ncurses: " << curses_version() << " (compiled with " << NCURSES_VERSION << ")" << std::endl;
		std::cout << "libcurl: " << curl_version()  << " (compiled with " << LIBCURL_VERSION << ")" << std::endl;
		std::cout << "SQLite: " << sqlite3_libversion() << " (compiled with " << SQLITE_VERSION << ")" << std::endl;
		std::cout << "libxml2: compiled with " << LIBXML_DOTTED_VERSION << std::endl << std::endl;
	} else {
		std::cout << LICENSE_str << std::endl;
	}

	::exit(EXIT_SUCCESS);
}