void controller::enqueue_items(std::shared_ptr<rss_feed> feed) {
	if (!cfg.get_configvalue_as_bool("podcast-auto-enqueue"))
		return;
	std::lock_guard<std::mutex> lock(feed->item_mutex);
	for (auto item : feed->items()) {
		if (!item->enqueued() && item->enclosure_url().length() > 0) {
			LOG(level::DEBUG, "controller::enqueue_items: enclosure_url = `%s' enclosure_type = `%s'", item->enclosure_url(), item->enclosure_type());
			if (is_valid_podcast_type(item->enclosure_type()) && utils::is_http_url(item->enclosure_url())) {
				LOG(level::INFO, "controller::enqueue_items: enqueuing `%s'", item->enclosure_url());
				enqueue_url(item->enclosure_url(), feed);
				item->set_enqueued(true);
				rsscache->update_rssitem_unread_and_enqueued(item, feed->rssurl());
			}
		}
	}
}