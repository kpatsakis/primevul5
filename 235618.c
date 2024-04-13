XMLNode::set_property(const char* name, const string& value)
{
	XMLPropertyIterator iter = _proplist.begin();

	while (iter != _proplist.end()) {
		if ((*iter)->name() == name) {
			(*iter)->set_value (value);
			return *iter;
		}
		++iter;
	}

	XMLProperty* new_property = new XMLProperty(name, value);

	if (!new_property) {
		return 0;
	}

	_proplist.insert(_proplist.end(), new_property);

	return new_property;
}