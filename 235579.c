XMLNode::property(const string& name)
{
	XMLPropertyIterator iter = _proplist.begin();

	while (iter != _proplist.end()) {
		if ((*iter)->name() == name) {
			return *iter;
		}
		++iter;
	}

	return 0;
}