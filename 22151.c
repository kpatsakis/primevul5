controller::~controller() {
	delete rsscache;
	delete urlcfg;
	delete api;

	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	for (auto feed : feeds) {
		std::lock_guard<std::mutex> lock(feed->item_mutex);
		feed->clear_items();
	}
	feeds.clear();
}