XMLNode::set_content(const string& c)
{
	if (c.empty()) {
		_is_content = false;
	} else {
		_is_content = true;
	}

	_content = c;

	return _content;
}