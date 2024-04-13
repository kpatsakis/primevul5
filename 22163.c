unsigned int controller::get_pos_of_next_unread(unsigned int pos) {
	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	for (pos++; pos < feeds.size(); pos++) {
		if (feeds[pos]->unread_item_count() > 0)
			break;
	}
	return pos;
}