gtime2generalTime(time_t gtime, char *str_time, size_t str_time_size)
{
	size_t ret;
	struct tm _tm;
	
	if (gtime == (time_t)-1
#if SIZEOF_LONG == 8
		|| gtime >= 253402210800
#endif
	 ) {
        	snprintf(str_time, str_time_size, "99991231235959Z");
        	return 0;
	}

	if (!gmtime_r(&gtime, &_tm)) {
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}

	ret = strftime(str_time, str_time_size, "%Y%m%d%H%M%SZ", &_tm);
	if (!ret) {
		gnutls_assert();
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}


	return 0;
}