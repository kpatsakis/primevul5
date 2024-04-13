unsigned int controller::get_feed_count_per_tag(const std::string& tag) {
	unsigned int count = 0;
	std::lock_guard<std::mutex> feedslock(feeds_mutex);

	for (auto feed : feeds) {
		if (feed->matches_tag(tag)) {
			count++;
		}
	}

	return count;
}