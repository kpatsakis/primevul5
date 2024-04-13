static int oidc_parse_expires_in(request_rec *r, const char *expires_in) {
	if (expires_in != NULL) {
		char *ptr = NULL;
		long number = strtol(expires_in, &ptr, 10);
		if (number <= 0) {
			oidc_warn(r,
					"could not convert \"expires_in\" value (%s) to a number",
					expires_in);
			return -1;
		}
		return number;
	}
	return -1;
}