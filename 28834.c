int dns_str_to_dn_label(const char *str, int str_len, char *dn, int dn_len)
{
	int i, offset;

	if (dn_len < str_len + 1)
		return -1;

	/* First byte of dn will be used to store the length of the first
	 * label */
	offset = 0;
	for (i = 0; i < str_len; ++i) {
		if (str[i] == '.') {
			/* 2 or more consecutive dots is invalid */
			if (i == offset)
				return -1;

			dn[offset] = (i - offset);
			offset = i+1;
			continue;
		}
		dn[i+1] = str[i];
	}
	dn[offset] = (i - offset - 1);
	dn[i] = '\0';
	return i;
}
