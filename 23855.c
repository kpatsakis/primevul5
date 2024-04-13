const char *cgit_loginurl(void)
{
	static const char *login_url;
	if (!login_url)
		login_url = fmtalloc("%s?p=login", cgit_rooturl());
	return login_url;
}