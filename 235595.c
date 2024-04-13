XMLNode::add_child_nocopy(XMLNode& n)
{
	_children.insert(_children.end(), &n);
}