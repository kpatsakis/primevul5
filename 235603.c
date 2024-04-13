XMLNode::XMLNode(const XMLNode& from)
{
	_proplist.reserve (PROPERTY_RESERVE_COUNT);
	*this = from;
}