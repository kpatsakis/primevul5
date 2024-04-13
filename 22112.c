std::string quote_empty(const std::string& input) {
	if (input.empty()) {
		return "''";
	} else {
		return input;
	}
}