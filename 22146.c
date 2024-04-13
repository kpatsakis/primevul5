std::string controller::write_temporary_item(std::shared_ptr<rss_item> item) {
	char filename[_POSIX_PATH_MAX];
	snprintf(filename, sizeof(filename), "/tmp/newsbeuter-article.XXXXXX");
	int fd = mkstemp(filename);
	if (fd != -1) {
		write_item(item, filename);
		close(fd);
		return std::string(filename);
	} else {
		return "";
	}
}