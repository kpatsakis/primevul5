const char *x11_flags2str(uint16_t flags)
{
	if (flags & X11_FORWARD_ALL)
		return "all";
	if (flags & X11_FORWARD_BATCH)
		return "batch";
	if (flags & X11_FORWARD_FIRST)
		return "first";
	if (flags & X11_FORWARD_LAST)
		return "last";

	return "unset";
}