void controller::rec_find_rss_outlines(xmlNode * node, std::string tag) {
	while (node) {
		std::string newtag = tag;


		if (strcmp((const char *)node->name, "outline")==0) {
			char * url = (char *)xmlGetProp(node, (const xmlChar *)"xmlUrl");
			if (!url) {
				url = (char *)xmlGetProp(node, (const xmlChar *)"url");
			}

			if (url) {
				LOG(level::DEBUG,"OPML import: found RSS outline with url = %s",url);

				std::string nurl = std::string(url);

				// Liferea uses a pipe to signal feeds read from the output of
				// a program in its OPMLs. Convert them to our syntax.
				if (*url == '|') {
					nurl = strprintf::fmt("exec:%s", url+1);
					LOG(level::DEBUG,"OPML import: liferea-style url %s converted to %s", url, nurl);
				}

				// Handle OPML filters.
				char * filtercmd = (char *)xmlGetProp(node, (const xmlChar *)"filtercmd");
				if (filtercmd) {
					LOG(level::DEBUG,"OPML import: adding filter command %s to url %s", filtercmd, nurl);
					nurl.insert(0, strprintf::fmt("filter:%s:", filtercmd));
					xmlFree(filtercmd);
				}

				xmlFree(url);
				// Filters and scripts may have arguments, so, quote them when needed.
				url = (char*) xmlStrdup((const xmlChar*)utils::quote_if_necessary(nurl).c_str());
				assert(url);

				bool found = false;

				LOG(level::DEBUG, "OPML import: size = %u", urlcfg->get_urls().size());
				if (urlcfg->get_urls().size() > 0) {
					for (auto u : urlcfg->get_urls()) {
						if (u == url) {
							found = true;
						}
					}
				}

				if (!found) {
					LOG(level::DEBUG,"OPML import: added url = %s",url);
					urlcfg->get_urls().push_back(std::string(url));
					if (tag.length() > 0) {
						LOG(level::DEBUG, "OPML import: appending tag %s to url %s", tag, url);
						urlcfg->get_tags(url).push_back(tag);
					}
				} else {
					LOG(level::DEBUG,"OPML import: url = %s is already in list",url);
				}
				xmlFree(url);
			} else {
				char * text = (char *)xmlGetProp(node, (const xmlChar *)"text");
				if (!text)
					text = (char *)xmlGetProp(node, (const xmlChar *)"title");
				if (text) {
					if (newtag.length() > 0) {
						newtag.append("/");
					}
					newtag.append(text);
					xmlFree(text);
				}
			}
		}
		rec_find_rss_outlines(node->children, newtag);

		node = node->next;
	}
}