void controller::reload_urls_file() {
	urlcfg->reload();
	std::vector<std::shared_ptr<rss_feed>> new_feeds;
	unsigned int i = 0;

	for (auto url : urlcfg->get_urls()) {
		bool found = false;
		for (auto feed : feeds) {
			if (url == feed->rssurl()) {
				found = true;
				feed->set_tags(urlcfg->get_tags(url));
				feed->set_order(i);
				new_feeds.push_back(feed);
				break;
			}
		}
		if (!found) {
			try {
				bool ignore_disp = (cfg.get_configvalue("ignore-mode") == "display");
				std::shared_ptr<rss_feed> new_feed = rsscache->internalize_rssfeed(url, ignore_disp ? &ign : nullptr);
				new_feed->set_tags(urlcfg->get_tags(url));
				new_feed->set_order(i);
				new_feeds.push_back(new_feed);
			} catch(const dbexception& e) {
				LOG(level::ERROR, "controller::reload_urls_file: caught exception: %s", e.what());
				throw;
			}
		}
		i++;
	}

	v->set_tags(urlcfg->get_alltags());

	{
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		feeds = new_feeds;
	}

	sort_feeds();

	update_feedlist();
}