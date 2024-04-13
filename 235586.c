XMLNode::operator== (const XMLNode& other) const
{
	if (is_content () != other.is_content ()) {
		return false;
	}

	if (is_content ()) {
		if (content () != other.content ()) {
			return false;
		}
	} else {
		if (name () != other.name ()) {
			return false;
		}
	}

	XMLPropertyList const& other_properties = other.properties ();

	if (_proplist.size () != other_properties.size ()) {
		return false;
	}

	XMLPropertyConstIterator our_prop_iter = _proplist.begin();
	XMLPropertyConstIterator other_prop_iter = other_properties.begin();

	while (our_prop_iter != _proplist.end ()) {
		XMLProperty const* our_prop = *our_prop_iter;
		XMLProperty const* other_prop = *other_prop_iter;
		if (our_prop->name () != other_prop->name () || our_prop->value () != other_prop->value ()) {
			return false;
		}
		++our_prop_iter;
		++other_prop_iter;
	}

	XMLNodeList const& other_children = other.children();

	if (_children.size() != other_children.size()) {
		return false;
	}

	XMLNodeConstIterator our_child_iter = _children.begin ();
	XMLNodeConstIterator other_child_iter = other_children.begin ();

	while (our_child_iter != _children.end()) {
		XMLNode const* our_child = *our_child_iter;
		XMLNode const* other_child = *other_child_iter;

		if (*our_child != *other_child) {
			return false;
		}
		++our_child_iter;
		++other_child_iter;
	}
	return true;
}