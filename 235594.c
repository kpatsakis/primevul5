XMLNode::clear_lists ()
{
	XMLNodeIterator curchild;
	XMLPropertyIterator curprop;

	_selected_children.clear ();

	for (curchild = _children.begin(); curchild != _children.end(); ++curchild) {
		delete *curchild;
	}

	_children.clear ();

	for (curprop = _proplist.begin(); curprop != _proplist.end(); ++curprop) {
		delete *curprop;
	}

	_proplist.clear ();
}