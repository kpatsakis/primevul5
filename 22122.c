void controller::mark_all_read(const std::string& feedurl) {
	try {
		rsscache->mark_all_read(feedurl);
	} catch (const dbexception& e) {
		v->show_error(strprintf::fmt(_("Error: couldn't mark all feeds read: %s"), e.what()));
		return;
	}

	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	for (const auto& feed : feeds) {
		std::lock_guard<std::mutex> lock(feed->item_mutex);

		if (feedurl.length() > 0 && feed->rssurl() != feedurl)
			continue;

		if (feed->total_item_count() > 0) {
			if (api) {
				api->mark_all_read(feed->rssurl());
			}
			for (auto item : feed->items()) {
				item->set_unread_nowrite(false);
			}
		}

		// no point in going on - there is only one feed with a given URL
		if (feedurl.length() > 0) break;
	}
}