void controller::write_item(std::shared_ptr<rss_item> item, std::ostream& ostr) {
	std::vector<std::pair<LineType, std::string>> lines;
	std::vector<linkpair> links; // not used

	std::string title(_("Title: "));
	title.append(item->title());
	lines.push_back(std::make_pair(LineType::wrappable, title));

	std::string author(_("Author: "));
	author.append(item->author());
	lines.push_back(std::make_pair(LineType::wrappable, author));

	std::string date(_("Date: "));
	date.append(item->pubDate());
	lines.push_back(std::make_pair(LineType::wrappable, date));

	std::string link(_("Link: "));
	link.append(item->link());
	lines.push_back(std::make_pair(LineType::softwrappable, link));

	if (item->enclosure_url() != "") {
		std::string dlurl(_("Podcast Download URL: "));
		dlurl.append(item->enclosure_url());
		lines.push_back(std::make_pair(LineType::softwrappable, dlurl));
	}

	lines.push_back(std::make_pair(LineType::wrappable, std::string("")));

	htmlrenderer rnd(true);
	rnd.render(item->description(), lines, links, item->feedurl());
	textformatter txtfmt;
	txtfmt.add_lines(lines);

	unsigned int width = cfg.get_configvalue_as_int("text-width");
	if (width == 0)
		width = 80;
	ostr << txtfmt.format_text_plain(width) << std::endl;
}