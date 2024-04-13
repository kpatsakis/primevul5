long ssl3_default_timeout(void)
	{
	/* 2 hours, the 24 hours mentioned in the SSLv3 spec
	 * is way too long for http, the cache would over fill */
	return(60*60*2);
	}
