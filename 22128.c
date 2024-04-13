std::shared_ptr<rss_feed> controller::get_feed_by_url(const std::string& feedurl) {
	for (auto feed : feeds) {
		if (feedurl == feed->rssurl())
			return feed;
	}
	LOG(level::ERROR, "controller:get_feed_by_url failed for %s", feedurl);
	return std::shared_ptr<rss_feed>();
}