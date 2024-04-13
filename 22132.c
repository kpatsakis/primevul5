std::string controller::generate_enqueue_filename(const std::string& url, std::shared_ptr<rss_feed> feed) {
	std::string dlformat = cfg.get_configvalue("download-path");
	if (dlformat[dlformat.length()-1] != NEWSBEUTER_PATH_SEP[0])
		dlformat.append(NEWSBEUTER_PATH_SEP);

	fmtstr_formatter fmt;
	fmt.register_fmt('n', feed->title());
	fmt.register_fmt('h', get_hostname_from_url(url));

	std::string dlpath = fmt.do_format(dlformat);

	char buf[2048];
	snprintf(buf, sizeof(buf), "%s", url.c_str());
	char * base = basename(buf);
	if (!base || strlen(base) == 0) {
		char lbuf[128];
		time_t t = time(nullptr);
		strftime(lbuf, sizeof(lbuf), "%Y-%b-%d-%H%M%S.unknown", localtime(&t));
		dlpath.append(lbuf);
	} else {
		dlpath.append(base);
	}
	return dlpath;
}