std::string controller::prepare_message(unsigned int pos, unsigned int max) {
	if (max > 0) {
		return strprintf::fmt("(%u/%u) ", pos, max);
	}
	return "";
}