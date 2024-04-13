void controller::export_read_information(const std::string& readinfofile) {
	std::vector<std::string> guids = rsscache->get_read_item_guids();

	std::fstream f;
	f.open(readinfofile.c_str(), std::fstream::out);
	if (f.is_open()) {
		for (auto guid : guids) {
			f << guid << std::endl;
		}
	}
}