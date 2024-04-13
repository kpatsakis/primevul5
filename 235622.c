XMLTree::XMLTree(const string& fn, bool validate)
	: _filename(fn)
	, _root(0)
	, _doc (0)
	, _compression(0)
{
	read_internal(validate);
}