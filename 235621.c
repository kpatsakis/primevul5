XMLNode::add_child(const char* n)
{
	return add_child_copy(XMLNode (n));
}