void controller::update_flags(std::shared_ptr<rss_item> item) {
	if (api) {
		api->update_article_flags(item->oldflags(), item->flags(), item->guid());
	}
	item->update_flags();
}