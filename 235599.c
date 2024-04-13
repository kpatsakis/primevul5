XMLTree::~XMLTree()
{
	delete _root;

	if (_doc) {
		xmlFreeDoc (_doc);
	}
}