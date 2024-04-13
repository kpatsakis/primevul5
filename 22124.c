void controller::update_visible_feeds() {
	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	v->update_visible_feeds(feeds);
}