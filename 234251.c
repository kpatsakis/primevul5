void html_status(int code, int more_headers)
{
	htmlf("Status: %d\n", code);
	if (!more_headers)
		html("\n");
}