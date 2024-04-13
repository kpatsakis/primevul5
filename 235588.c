XMLNode::dump (ostream& s, string p) const
{
	if (_is_content) {
		s << p << "  " << content() << "\n";
	} else {
		s << p << "<" << _name;
		for (XMLPropertyList::const_iterator i = _proplist.begin(); i != _proplist.end(); ++i) {
			s << " " << (*i)->name() << "=\"" << (*i)->value() << "\"";
		}
		s << ">\n";

		for (XMLNodeList::const_iterator i = _children.begin(); i != _children.end(); ++i) {
			(*i)->dump (s, p + "  ");
		}

		s << p << "</" << _name << ">\n";
	}
}