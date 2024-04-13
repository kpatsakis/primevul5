srs_parse_shortcut(srs_t *srs, char *buf, unsigned buflen, char *senduser)
{
	char	*srshash;
	char	*srsstamp;
	char	*srshost;
	char	*srsuser;
	int		 ret;

	if (strncasecmp(senduser, SRS0TAG, 4) == 0) {
		srshash = senduser + 5;
		if (!STRINGP(srshash))
			return SRS_ENOSRS0HASH;
		srsstamp = strchr(srshash, SRSSEP);
		if (!STRINGP(srsstamp))
			return SRS_ENOSRS0STAMP;
		*srsstamp++ = '\0';
		srshost = strchr(srsstamp, SRSSEP);
		if (!STRINGP(srshost))
			return SRS_ENOSRS0HOST;
		*srshost++ = '\0';
		srsuser = strchr(srshost, SRSSEP);
		if (!STRINGP(srsuser))
			return SRS_ENOSRS0USER;
		*srsuser++ = '\0';
		ret = srs_timestamp_check(srs, srsstamp);
		if (ret != SRS_SUCCESS)
			return ret;
		ret = srs_hash_check(srs, srshash, 3, srsstamp,
						srshost, srsuser);
		if (ret != SRS_SUCCESS)
			return ret;
		snprintf(buf, buflen, "%s@%s", srsuser, srshost);
		return SRS_SUCCESS;
	}

	return SRS_ENOTSRSADDRESS;
}