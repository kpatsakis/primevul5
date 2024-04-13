uint16_t x11_str2flags(const char *str)
{
	uint16_t flags = 0;

	if (!xstrcasecmp(str, "all"))
		flags |= X11_FORWARD_ALL;
	if (!xstrcasecmp(str, "batch"))
		flags |= X11_FORWARD_BATCH;
	if (!xstrcasecmp(str, "first"))
		flags |= X11_FORWARD_FIRST;
	if (!xstrcasecmp(str, "last"))
		flags |= X11_FORWARD_LAST;

	return flags;
}