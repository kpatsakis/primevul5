static int is_whitelisted(char c, const char *white)
{
	if ((c >= '0' && c <= '9') ||
	    (c >= 'A' && c <= 'Z') ||
	    (c >= 'a' && c <= 'z') ||
	    strchr("#+-.:=@_", c) != NULL ||
	    (white != NULL && strchr(white, c) != NULL))
		return 1;
	return 0;
}