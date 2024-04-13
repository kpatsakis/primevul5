int dns_hostname_validation(const char *string, char **err)
{
	const char *c, *d;
	int i;

	if (strlen(string) > DNS_MAX_NAME_SIZE) {
		if (err)
			*err = DNS_TOO_LONG_FQDN;
		return 0;
	}

	c = string;
	while (*c) {
		d = c;

		i = 0;
		while (*d != '.' && *d && i <= DNS_MAX_LABEL_SIZE) {
			i++;
			if (!((*d == '-') || (*d == '_') ||
			      ((*d >= 'a') && (*d <= 'z')) ||
			      ((*d >= 'A') && (*d <= 'Z')) ||
			      ((*d >= '0') && (*d <= '9')))) {
				if (err)
					*err = DNS_INVALID_CHARACTER;
				return 0;
			}
			d++;
		}

		if ((i >= DNS_MAX_LABEL_SIZE) && (d[i] != '.')) {
			if (err)
				*err = DNS_LABEL_TOO_LONG;
			return 0;
		}

		if (*d == '\0')
			goto out;

		c = ++d;
	}
 out:
	return 1;
}
