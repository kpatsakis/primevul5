XMLNode::property(const char* name) const
{
	XMLPropertyConstIterator iter = _proplist.begin();

	while (iter != _proplist.end()) {
		if ((*iter)->name() == name) {
			return *iter;
		}
		++iter;
	}

	return 0;
}