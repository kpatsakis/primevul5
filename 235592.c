XMLTree::XMLTree(const XMLTree* from)
	: _filename(from->filename())
	, _root(new XMLNode(*from->root()))
	, _doc (xmlCopyDoc (from->_doc, 1))
	, _compression(from->compression())
{

}