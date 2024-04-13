XMLNode::remove_nodes(const string& n)
{
	XMLNodeIterator i = _children.begin();
	while (i != _children.end()) {
		if ((*i)->name() == n) {
			i = _children.erase (i);
		} else {
			++i;
		}
	}
}