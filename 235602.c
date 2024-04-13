XMLNode::operator= (const XMLNode& from)
{
	if (&from == this) {
		return *this;
	}

	clear_lists ();

	_name = from.name ();
	set_content (from.content ());

	const XMLPropertyList& props = from.properties ();

	for (XMLPropertyConstIterator prop_iter = props.begin (); prop_iter != props.end (); ++prop_iter) {
		set_property ((*prop_iter)->name ().c_str (), (*prop_iter)->value ());
	}

	const XMLNodeList& nodes = from.children ();
	for (XMLNodeConstIterator child_iter = nodes.begin (); child_iter != nodes.end (); ++child_iter) {
		add_child_copy (**child_iter);
	}

	return *this;
}