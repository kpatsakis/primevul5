int dns_dn_label_to_str(const char *dn, int dn_len, char *str, int str_len)
{
	char *ptr;
	int i, sz;

	if (str_len < dn_len - 1)
		return -1;

	ptr = str;
	for (i = 0; i < dn_len-1; ++i) {
		sz = dn[i];
		if (i)
			*ptr++ = '.';
		memcpy(ptr, dn+i+1, sz);
		ptr += sz;
		i   += sz;
	}
	*ptr++ = '\0';
	return (ptr - str);
}
