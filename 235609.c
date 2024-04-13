XMLNode::add_content(const string& c)
{
	if (c.empty ()) {
		/* this would add a "</>" child, leading to invalid XML.
		 * Also in XML, empty string content is equivalent to no content.
		 */
		return NULL;
	}
	return add_child_copy(XMLNode (string(), c));
}