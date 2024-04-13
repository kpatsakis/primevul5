void controller::reload_indexes(const std::vector<int>& indexes, bool unattended) {
	scope_measure m1("controller::reload_indexes");
	unsigned int unread_feeds, unread_articles;
	compute_unread_numbers(unread_feeds, unread_articles);

	unsigned long size;
	{
		std::lock_guard<std::mutex> feedslock(feeds_mutex);
		size = feeds.size();
	}

	for (auto idx : indexes) {
		this->reload(idx, size, unattended);
	}

	unsigned int unread_feeds2, unread_articles2;
	compute_unread_numbers(unread_feeds2, unread_articles2);
	bool notify_always = cfg.get_configvalue_as_bool("notify-always");
	if (notify_always || unread_feeds2 != unread_feeds || unread_articles2 != unread_articles) {
		fmtstr_formatter fmt;
		fmt.register_fmt('f', std::to_string(unread_feeds2));
		fmt.register_fmt('n', std::to_string(unread_articles2));
		fmt.register_fmt('d', std::to_string(unread_articles2 - unread_articles));
		fmt.register_fmt('D', std::to_string(unread_feeds2 - unread_feeds));
		this->notify(fmt.do_format(cfg.get_configvalue("notify-format")));
	}
	if (!unattended)
		v->set_status("");
}