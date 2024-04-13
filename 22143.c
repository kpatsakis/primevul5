void controller::mark_deleted(const std::string& guid, bool b) {
	rsscache->mark_item_deleted(guid, b);
}