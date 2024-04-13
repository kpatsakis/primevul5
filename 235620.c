XMLNode::remove_nodes_and_delete(const string& n)
{
	XMLNodeIterator i = _children.begin();

	while (i != _children.end()) {
		if ((*i)->name() == n) {
			delete *i;
			i = _children.erase (i);
		} else {
			++i;
		}
	}
}