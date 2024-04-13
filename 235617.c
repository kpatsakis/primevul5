XMLNode::remove_property_recursively(const string& n)
{
	remove_property (n);
	for (XMLNodeIterator i = _children.begin(); i != _children.end(); ++i) {
		(*i)->remove_property_recursively (n);
	}
}