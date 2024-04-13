XMLNode::children(const string& n) const
{
	/* returns all children matching name */

	XMLNodeConstIterator cur;

	if (n.empty()) {
		return _children;
	}

	_selected_children.clear();

	for (cur = _children.begin(); cur != _children.end(); ++cur) {
		if ((*cur)->name() == n) {
			_selected_children.insert(_selected_children.end(), *cur);
		}
	}

	return _selected_children;
}