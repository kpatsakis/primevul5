std::vector<std::shared_ptr<rss_item>> controller::search_for_items(const std::string& query, std::shared_ptr<rss_feed> feed) {
	std::vector<std::shared_ptr<rss_item>> items;
	LOG(level::DEBUG, "controller::search_for_items: setting feed pointers");
	if (feed != nullptr && feed->rssurl().substr(0,6) == "query:") {
		for (auto item : feed->items()) {
			if (!item->deleted()
				&& (item->title().find(query) != std::string::npos
				    || item->description().find(query) != std::string::npos))
			{
				std::shared_ptr<rss_item> newitem(new rss_item(nullptr));
				newitem->set_guid(item->guid());
				newitem->set_title(item->title());
				newitem->set_author(item->author());
				newitem->set_link(item->link());

				newitem->set_pubDate(item->pubDate_timestamp());

				newitem->set_size(item->size());
				newitem->set_unread(item->unread());
				newitem->set_feedurl(item->feedurl());

				newitem->set_enclosure_url(item->enclosure_url());
				newitem->set_enclosure_type(item->enclosure_type());
				newitem->set_enqueued(item->enqueued());
				newitem->set_flags(item->flags());
				newitem->set_base(item->get_base());

				newitem->set_feedptr(item->get_feedptr());
				newitem->set_cache(get_cache());

				items.push_back(newitem);
			}
		}
	} else {
		items = rsscache->search_for_items(query, (feed != nullptr ? feed->rssurl() : ""));
		for (auto item : items) {
			item->set_feedptr(get_feed_by_url(item->feedurl()));
		}
	}
	return items;
}