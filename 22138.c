void controller::update_feedlist() {
	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	v->set_feedlist(feeds);
}