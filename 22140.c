void controller::write_item(std::shared_ptr<rss_item> item, const std::string& filename) {
	std::fstream f;
	f.open(filename.c_str(),std::fstream::out);
	if (!f.is_open())
		throw exception(errno);

	write_item(item, f);
}