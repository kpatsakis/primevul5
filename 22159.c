std::shared_ptr<rss_feed> controller::get_feed(unsigned int pos) {
	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	if (pos >= feeds.size()) {
		throw std::out_of_range(_("invalid feed index (bug)"));
	}
	std::shared_ptr<rss_feed> feed = feeds[pos];
	return feed;
}