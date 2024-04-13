void controller::sort_feeds() {
	std::lock_guard<std::mutex> feedslock(feeds_mutex);
	std::vector<std::string> sortmethod_info = utils::tokenize(cfg.get_configvalue("feed-sort-order"), "-");
	std::string sortmethod = sortmethod_info[0];
	std::string direction = "desc";
	if (sortmethod_info.size() > 1)
		direction = sortmethod_info[1];
	if (sortmethod == "none") {
		std::stable_sort(feeds.begin(), feeds.end(), [](std::shared_ptr<rss_feed> a, std::shared_ptr<rss_feed> b) {
			return a->get_order() < b->get_order();
		});
	} else if (sortmethod == "firsttag") {
		std::stable_sort(feeds.begin(), feeds.end(), [](std::shared_ptr<rss_feed> a, std::shared_ptr<rss_feed> b) {
			if (a->get_firsttag().length() == 0 || b->get_firsttag().length() == 0) {
				return a->get_firsttag().length() > b->get_firsttag().length();
			}
			return strcasecmp(a->get_firsttag().c_str(), b->get_firsttag().c_str()) < 0;
		});
	} else if (sortmethod == "title") {
		std::stable_sort(feeds.begin(), feeds.end(), [](std::shared_ptr<rss_feed> a, std::shared_ptr<rss_feed> b) {
			return strcasecmp(a->title().c_str(), b->title().c_str()) < 0;
		});
	} else if (sortmethod == "articlecount") {
		std::stable_sort(feeds.begin(), feeds.end(), [](std::shared_ptr<rss_feed> a, std::shared_ptr<rss_feed> b) {
			return a->total_item_count() < b->total_item_count();
		});
	} else if (sortmethod == "unreadarticlecount") {
		std::stable_sort(feeds.begin(), feeds.end(), [](std::shared_ptr<rss_feed> a, std::shared_ptr<rss_feed> b) {
			return a->unread_item_count() < b->unread_item_count();
		});
	}
	if (direction == "asc") {
		std::reverse(feeds.begin(), feeds.end());
	}
}