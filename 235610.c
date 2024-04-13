XMLNode::remove_node_and_delete(const string& n, const string& propname, const string& val)
{
	for (XMLNodeIterator i = _children.begin(); i != _children.end(); ++i) {
		if ((*i)->name() == n) {
			XMLProperty const * prop = (*i)->property (propname);
			if (prop && prop->value() == val) {
				delete *i;
				_children.erase (i);
				break;
			}
		}
	}
}