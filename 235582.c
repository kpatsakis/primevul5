XMLNode::remove_nodes_and_delete(const string& propname, const string& val)
{
	XMLNodeIterator i = _children.begin();
	XMLProperty const * prop;

	while (i != _children.end()) {
		prop = (*i)->property(propname);
		if (prop && prop->value() == val) {
			delete *i;
			i = _children.erase(i);
		} else {
			++i;
		}
	}
}