XMLNode::remove_property(const string& name)
{
	XMLPropertyIterator iter = _proplist.begin();

	while (iter != _proplist.end()) {
		if ((*iter)->name() == name) {
			XMLProperty* property = *iter;
			_proplist.erase (iter);
			delete property;
			break;
		}
		++iter;
	}
}