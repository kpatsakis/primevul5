void controller::compute_unread_numbers(unsigned int& unread_feeds, unsigned int& unread_articles) {
	unread_feeds = 0;
	unread_articles = 0;
	for (auto feed : feeds) {
		unsigned int items = feed->unread_item_count();
		if (items > 0) {
			++unread_feeds;
			unread_articles += items;
		}
	}
}