void controller::edit_urls_file() {
	const char * editor;

	editor = getenv("VISUAL");
	if (!editor)
		editor = getenv("EDITOR");
	if (!editor)
		editor = "vi";

	std::string cmdline = strprintf::fmt("%s \"%s\"", editor, utils::replace_all(url_file,"\"","\\\""));

	v->push_empty_formaction();
	stfl::reset();

	utils::run_interactively(cmdline, "controller::edit_urls_file");

	v->pop_current_formaction();

	reload_urls_file();
}