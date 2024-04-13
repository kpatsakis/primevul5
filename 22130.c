void controller::enqueue_url(const std::string& url, std::shared_ptr<rss_feed> feed) {
	bool url_found = false;
	std::fstream f;
	f.open(queue_file.c_str(), std::fstream::in);
	if (f.is_open()) {
		do {
			std::string line;
			getline(f, line);
			if (!f.eof() && line.length() > 0) {
				std::vector<std::string> fields = utils::tokenize_quoted(line);
				if (!fields.empty() && fields[0] == url) {
					url_found = true;
					break;
				}
			}
		} while (!f.eof());
		f.close();
	}
	if (!url_found) {
		f.open(queue_file.c_str(), std::fstream::app | std::fstream::out);
		std::string filename = generate_enqueue_filename(url, feed);
		f << url << " " << stfl::quote(filename) << std::endl;
		f.close();
	}
}