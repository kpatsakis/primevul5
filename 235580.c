XMLTree::read_buffer(const string& buffer, bool to_tree_doc)
{
	xmlDocPtr doc;

	_filename = "";

	delete _root;
	_root = 0;

	doc = xmlParseMemory(const_cast<char*>(buffer.c_str()), buffer.length());
	if (!doc) {
		return false;
	}

	_root = readnode(xmlDocGetRootElement(doc));
	if (to_tree_doc) {
		if (_doc) {
			xmlFreeDoc (_doc);
		}
		_doc = doc;
	} else {
		xmlFreeDoc (doc);
	}

	return true;
}