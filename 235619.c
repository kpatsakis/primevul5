XMLTree::set_compression(int c)
{
	if (c > 9) {
		c = 9;
	} else if (c < 0) {
		c = 0;
	}

	_compression = c;

	return _compression;
}