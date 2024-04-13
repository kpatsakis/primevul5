std::vector<std::shared_ptr<rss_feed>> controller::get_all_feeds() {
	std::vector<std::shared_ptr<rss_feed>> tmpfeeds;
	{
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		tmpfeeds = feeds;
	}
	return tmpfeeds;
}