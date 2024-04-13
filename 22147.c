void controller::save_feed(std::shared_ptr<rss_feed> feed, unsigned int pos) {
	if (!feed->is_empty()) {
		LOG(level::DEBUG, "controller::save_feed: feed is nonempty, saving");
		rsscache->externalize_rssfeed(feed, ign.matches_resetunread(feed->rssurl()));
		LOG(level::DEBUG, "controller::save_feed: after externalize_rssfeed");

		bool ignore_disp = (cfg.get_configvalue("ignore-mode") == "display");
		feed = rsscache->internalize_rssfeed(feed->rssurl(), ignore_disp ? &ign : nullptr);
		LOG(level::DEBUG, "controller::save_feed: after internalize_rssfeed");
		feed->set_tags(urlcfg->get_tags(feed->rssurl()));
		{
			unsigned int order = feeds[pos]->get_order();
			std::lock_guard<std::mutex> itemlock(feeds[pos]->item_mutex);
			feeds[pos]->clear_items();
			feed->set_order(order);
		}
		feeds[pos] = feed;
		v->notify_itemlist_change(feeds[pos]);
	} else {
		LOG(level::DEBUG, "controller::save_feed: feed is empty, not saving");
	}
}