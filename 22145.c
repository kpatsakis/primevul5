void controller::reload(unsigned int pos, unsigned int max, bool unattended, curl_handle *easyhandle) {
	LOG(level::DEBUG, "controller::reload: pos = %u max = %u", pos, max);
	if (pos < feeds.size()) {
		std::shared_ptr<rss_feed> oldfeed = feeds[pos];
		std::string errmsg;
		if (!unattended)
			v->set_status(strprintf::fmt(_("%sLoading %s..."), prepare_message(pos+1, max), utils::censor_url(oldfeed->rssurl())));

		bool ignore_dl = (cfg.get_configvalue("ignore-mode") == "download");

		rss_parser parser(oldfeed->rssurl(), rsscache, &cfg, ignore_dl ? &ign : nullptr, api);
		parser.set_easyhandle(easyhandle);
		LOG(level::DEBUG, "controller::reload: created parser");
		try {
			oldfeed->set_status(dl_status::DURING_DOWNLOAD);
			std::shared_ptr<rss_feed> newfeed = parser.parse();
			if (newfeed->total_item_count() > 0) {
				std::lock_guard<std::mutex> feedslock(feeds_mutex);
				save_feed(newfeed, pos);

				newfeed->clear_items();

				bool ignore_disp = (cfg.get_configvalue("ignore-mode") == "display");
				std::shared_ptr<rss_feed> feed = rsscache->internalize_rssfeed(oldfeed->rssurl(), ignore_disp ? &ign : nullptr);
				feed->set_tags(urlcfg->get_tags(oldfeed->rssurl()));
				feed->set_order(oldfeed->get_order());
				feeds[pos] = feed;
				enqueue_items(feed);

				oldfeed->clear_items();

				v->notify_itemlist_change(feeds[pos]);
				if (!unattended) {
					v->set_feedlist(feeds);
				}
			} else {
				LOG(level::DEBUG, "controller::reload: feed is empty");
			}
			oldfeed->set_status(dl_status::SUCCESS);
			v->set_status("");
		} catch (const dbexception& e) {
			errmsg = strprintf::fmt(_("Error while retrieving %s: %s"), utils::censor_url(oldfeed->rssurl()), e.what());
		} catch (const std::string& emsg) {
			errmsg = strprintf::fmt(_("Error while retrieving %s: %s"), utils::censor_url(oldfeed->rssurl()), emsg);
		} catch (rsspp::exception& e) {
			errmsg = strprintf::fmt(_("Error while retrieving %s: %s"), utils::censor_url(oldfeed->rssurl()), e.what());
		}
		if (errmsg != "") {
			oldfeed->set_status(dl_status::DL_ERROR);
			v->set_status(errmsg);
			LOG(level::USERERROR, "%s", errmsg);
		}
	} else {
		v->show_error(_("Error: invalid feed!"));
	}
}