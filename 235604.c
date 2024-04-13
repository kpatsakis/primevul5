XMLNode::add_child_copy(const XMLNode& n)
{
	XMLNode *copy = new XMLNode(n);
	_children.insert(_children.end(), copy);
	return copy;
}