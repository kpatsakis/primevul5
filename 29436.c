static int addrcmp(const void *_a, const void *_b)
{
	const struct address *a = _a, *b = _b;
	return b->sortkey - a->sortkey;
}
