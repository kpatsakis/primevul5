XMLNode::attribute_value()
{
	XMLNodeList children = this->children();
	if (_is_content)
		throw XMLException("XMLNode: attribute_value failed (is_content) for requested node: " + name());

	if (children.size() != 1)
		throw XMLException("XMLNode: attribute_value failed (children.size != 1) for requested node: " + name());

	XMLNode* child = *(children.begin());
	if (!child->is_content())
		throw XMLException("XMLNode: attribute_value failed (!child->is_content()) for requested node: " + name());

	return child->content();
}