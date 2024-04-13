XMLNode::has_property_with_value (const string& name, const string& value) const
{
	XMLPropertyConstIterator iter = _proplist.begin();

	while (iter != _proplist.end()) {
		if ((*iter)->name() == name && (*iter)->value() == value) {
			return true;
		}
		++iter;
	}
	return false;
}