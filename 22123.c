void controller::usage(char * argv0) {
	auto msg =
	    strprintf::fmt(_("%s %s\nusage: %s [-i <file>|-e] [-u <urlfile>] "
	    "[-c <cachefile>] [-x <command> ...] [-h]\n"),
	    PROGRAM_NAME,
	    PROGRAM_VERSION,
	    argv0);
	std::cout << msg;

	struct arg {
		const char name;
		const std::string longname;
		const std::string params;
		const std::string desc;
	};

	static const std::vector<arg> args = {
		{ 'e', "export-to-opml"  , ""                , _s("export OPML feed to stdout") }                                                 ,
		{ 'r', "refresh-on-start", ""                , _s("refresh feeds on start") }                                                     ,
		{ 'i', "import-from-opml", _s("<file>")      , _s("import OPML file") }                                                           ,
		{ 'u', "url-file"        , _s("<urlfile>")   , _s("read RSS feed URLs from <urlfile>") }                                          ,
		{ 'c', "cache-file"      , _s("<cachefile>") , _s("use <cachefile> as cache file") }                                              ,
		{ 'C', "config-file"     , _s("<configfile>"), _s("read configuration from <configfile>") }                                       ,
		{ 'X', "vacuum"          , ""                , _s("compact the cache") }                                                  ,
		{ 'x', "execute"         , _s("<command>..."), _s("execute list of commands") }                                                   ,
		{ 'q', "quiet"           , ""                , _s("quiet startup") }                                                              ,
		{ 'v', "version"         , ""                , _s("get version information") }                                                    ,
		{ 'l', "log-level"       , _s("<loglevel>")  , _s("write a log with a certain loglevel (valid values: 1 to 6)") }                 ,
		{ 'd', "log-file"        , _s("<logfile>")   , _s("use <logfile> as output log file") }                                           ,
		{ 'E', "export-to-file"  , _s("<file>")      , _s("export list of read articles to <file>") }                                     ,
		{ 'I', "import-from-file", _s("<file>")      , _s("import list of read articles from <file>") }                                   ,
		{ 'h', "help"            , ""                , _s("this help") }
	};

	for (const auto & a : args) {
		std::string longcolumn("-");
		longcolumn += a.name;
		longcolumn += ", --" + a.longname;
		longcolumn += a.params.size() > 0 ? "=" + a.params : "";
		std::cout << "\t" << longcolumn;
		for (unsigned int j = 0; j < utils::gentabs(longcolumn); j++) {
			std::cout << "\t";
		}
		std::cout << a.desc << std::endl;
	}

	::exit(EXIT_FAILURE);
}