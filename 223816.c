srs_compile_guarded(srs_t *srs,
				char *buf, int buflen,
				char *sendhost, char *senduser,
				const char *aliashost) {
	char	*srshost;
	char	*srsuser;
	char	*srshash;
	int		 len;
	int		 ret;

	if ((strncasecmp(senduser, SRS1TAG, 4) == 0) &&
		(strchr(srs_separators, senduser[4]) != NULL)) {
		/* Used as a temporary convenience var */
		srshash = senduser + 5;
		if (*srshash == '\0')
			return SRS_ENOSRS1HASH;
		/* Used as a temporary convenience var */
		srshost = strchr(srshash, SRSSEP);
		if (!STRINGP(srshost))
			return SRS_ENOSRS1HOST;
		*srshost++ = '\0';
		srsuser = strchr(srshost, SRSSEP);
		if (!STRINGP(srsuser))
			return SRS_ENOSRS1USER;
		*srsuser++ = '\0';
		srshash = alloca(srs->hashlength + 1);
		ret = srs_hash_create(srs, srshash, 2, srshost, srsuser);
		if (ret != SRS_SUCCESS)
			return ret;
		len = strlen(SRS1TAG) + 1 +
			srs->hashlength + 1 +
				strlen(srshost) + 1 + strlen(srsuser)
			+ 1 + strlen(aliashost);
		if (len >= buflen)
			return SRS_EBUFTOOSMALL;
		sprintf(buf, SRS1TAG "%c%s%c%s%c%s@%s", srs->separator,
						srshash, SRSSEP,
							srshost, SRSSEP, srsuser,
								aliashost);
		return SRS_SUCCESS;
	}
	else if ((strncasecmp(senduser, SRS0TAG, 4) == 0) &&
		(strchr(srs_separators, senduser[4]) != NULL)) {
		srsuser = senduser + 4;
		srshost = sendhost;
		srshash = alloca(srs->hashlength + 1);
		ret = srs_hash_create(srs, srshash, 2, srshost, srsuser);
		if (ret != SRS_SUCCESS)
			return ret;
		len = strlen(SRS1TAG) + 1 +
			srs->hashlength + 1 +
				strlen(srshost) + 1 + strlen(srsuser)
			+ 1 + strlen(aliashost);
		if (len >= buflen)
			return SRS_EBUFTOOSMALL;
		sprintf(buf, SRS1TAG "%c%s%c%s%c%s@%s", srs->separator,
						srshash, SRSSEP,
							srshost, SRSSEP, srsuser,
								aliashost);
	}
	else {
		return srs_compile_shortcut(srs, buf, buflen,
						sendhost, senduser, aliashost);
	}

	return SRS_SUCCESS;
}