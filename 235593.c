XMLNode::XMLNode(const string& n)
	: _name(n)
	, _is_content(false)
{
	_proplist.reserve (PROPERTY_RESERVE_COUNT);
}