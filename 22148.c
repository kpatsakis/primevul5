void controller::import_opml(const std::string& filename) {
	xmlDoc * doc = xmlReadFile(filename.c_str(), nullptr, 0);
	if (doc == nullptr) {
		std::cout
			<< strprintf::fmt(_("An error occurred while parsing %s."), filename)
			<< std::endl;
		return;
	}

	xmlNode * root = xmlDocGetRootElement(doc);

	for (xmlNode * node = root->children; node != nullptr; node = node->next) {
		if (strcmp((const char *)node->name, "body")==0) {
			LOG(level::DEBUG, "import_opml: found body");
			rec_find_rss_outlines(node->children, "");
			urlcfg->write_config();
		}
	}

	xmlFreeDoc(doc);
	std::cout << strprintf::fmt(_("Import of %s finished."), filename) << std::endl;
}