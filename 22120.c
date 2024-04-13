void controller::reload_all(bool unattended) {
	unsigned int unread_feeds, unread_articles;
	compute_unread_numbers(unread_feeds, unread_articles);
	unsigned int num_threads = cfg.get_configvalue_as_int("reload-threads");
	time_t t1, t2, dt;

	unsigned int size;

	{
		std::lock_guard<std::mutex> feedlock(feeds_mutex);
		for (auto feed : feeds) {
			feed->reset_status();
		}
		size = feeds.size();
	}

	if (num_threads < 1)
		num_threads = 1;

	if (num_threads > size) {
		num_threads = size;
	}


	t1 = time(nullptr);

	LOG(level::DEBUG,"controller::reload_all: starting with reload all...");
	if (num_threads <= 1) {
		this->reload_range(0, size-1, size, unattended);
	} else {
		std::vector<std::pair<unsigned int, unsigned int>> partitions = utils::partition_indexes(0, size-1, num_threads);
		std::vector<std::thread> threads;
		LOG(level::DEBUG, "controller::reload_all: starting reload threads...");
		for (unsigned int i=0; i<num_threads-1; i++) {
			threads.push_back(std::thread(reloadrangethread(this, partitions[i].first, partitions[i].second, size, unattended)));
		}
		LOG(level::DEBUG, "controller::reload_all: starting my own reload...");
		this->reload_range(partitions[num_threads-1].first, partitions[num_threads-1].second, size, unattended);
		LOG(level::DEBUG, "controller::reload_all: joining other threads...");
		for (size_t i=0; i<threads.size(); i++) {
			threads[i].join();
		}
	}

	// refresh query feeds (update and sort)
	LOG(level::DEBUG, "controller::reload_all: refresh query feeds");
	for (auto feed : feeds) {
		v->prepare_query_feed(feed);
	}
	v->force_redraw();

	sort_feeds();
	update_feedlist();

	t2 = time(nullptr);
	dt = t2 - t1;
	LOG(level::INFO, "controller::reload_all: reload took %d seconds", dt);

	unsigned int unread_feeds2, unread_articles2;
	compute_unread_numbers(unread_feeds2, unread_articles2);
	bool notify_always = cfg.get_configvalue_as_bool("notify-always");
	if (notify_always || unread_feeds2 > unread_feeds || unread_articles2 > unread_articles) {
		int article_count = unread_articles2 - unread_articles;
		int feed_count = unread_feeds2 - unread_feeds;

		LOG(level::DEBUG, "unread article count: %d", article_count);
		LOG(level::DEBUG, "unread feed count: %d", feed_count);

		fmtstr_formatter fmt;
		fmt.register_fmt('f', std::to_string(unread_feeds2));
		fmt.register_fmt('n', std::to_string(unread_articles2));
		fmt.register_fmt('d', std::to_string(article_count >= 0 ? article_count : 0));
		fmt.register_fmt('D', std::to_string(feed_count >= 0 ? feed_count : 0));
		this->notify(fmt.do_format(cfg.get_configvalue("notify-format")));
	}
}