void controller::mark_article_read(const std::string& guid, bool read) {
	if (api) {
		api->mark_article_read(guid, read);
	}
}