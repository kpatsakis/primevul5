srs_parse_guarded(srs_t *srs, char *buf, int buflen, char *senduser)
{
	char	*srshash;
	char	*srshost;
	char	*srsuser;
	int		 ret;

	if (strncasecmp(senduser, SRS1TAG, 4) == 0) {
		srshash = senduser + 5;
		if (!STRINGP(srshash))
			return SRS_ENOSRS1HASH;
		srshost = strchr(srshash, SRSSEP);
		if (!STRINGP(srshost))
			return SRS_ENOSRS1HOST;
		*srshost++ = '\0';
		srsuser = strchr(srshost, SRSSEP);
		if (!STRINGP(srsuser))
			return SRS_ENOSRS1USER;
		*srsuser++ = '\0';
		ret = srs_hash_check(srs, srshash, 2, srshost, srsuser);
		if (ret != SRS_SUCCESS)
			return ret;
		sprintf(buf, SRS0TAG "%s@%s", srsuser, srshost);
		return SRS_SUCCESS;
	}
	else {
		return srs_parse_shortcut(srs, buf, buflen, senduser);
	}
}