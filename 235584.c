XMLNode::XMLNode(const string& n, const string& c)
	: _name(n)
	, _is_content(true)
	, _content(c)
{
	_proplist.reserve (PROPERTY_RESERVE_COUNT);
}