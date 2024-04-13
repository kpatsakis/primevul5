void controller::mark_all_read(unsigned int pos) {
	if (pos < feeds.size()) {
		scope_measure m("controller::mark_all_read");
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		std::shared_ptr<rss_feed> feed = feeds[pos];
		if (feed->rssurl().substr(0,6) == "query:") {
			rsscache->mark_all_read(feed);
		} else {
			rsscache->mark_all_read(feed->rssurl());
			if (api) {
				api->mark_all_read(feed->rssurl());
			}
		}
		m.stopover("after rsscache->mark_all_read, before iteration over items");
		std::lock_guard<std::mutex> lock(feed->item_mutex);
		std::vector<std::shared_ptr<rss_item>>& items = feed->items();
		if (items.size() > 0) {
			bool notify = items[0]->feedurl() != feed->rssurl();
			LOG(level::DEBUG, "controller::mark_all_read: notify = %s", notify ? "yes" : "no");
			for (auto item : items) {
				item->set_unread_nowrite_notify(false, notify);
			}
		}
	}
}