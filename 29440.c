static int name_from_dns_search(struct address buf[static MAXADDRS], char canon[static 256], const char *name, int family)
{
	char search[256];
	struct resolvconf conf;
	size_t l, dots;
	char *p, *z;

	if (__get_resolv_conf(&conf, search, sizeof search) < 0) return -1;

	/* Count dots, suppress search when >=ndots or name ends in
	 * a dot, which is an explicit request for global scope. */
	for (dots=l=0; name[l]; l++) if (name[l]=='.') dots++;
	if (dots >= conf.ndots || name[l-1]=='.') *search = 0;

	/* This can never happen; the caller already checked length. */
	if (l >= 256) return EAI_NONAME;

	/* Name with search domain appended is setup in canon[]. This both
	 * provides the desired default canonical name (if the requested
	 * name is not a CNAME record) and serves as a buffer for passing
	 * the full requested name to name_from_dns. */
	memcpy(canon, name, l);
	canon[l] = '.';

	for (p=search; *p; p=z) {
		for (; isspace(*p); p++);
		for (z=p; *z && !isspace(*z); z++);
		if (z==p) break;
		if (z-p < 256 - l - 1) {
			memcpy(canon+l+1, p, z-p);
			canon[z-p+1+l] = 0;
			int cnt = name_from_dns(buf, canon, canon, family, &conf);
			if (cnt) return cnt;
		}
	}

	canon[l] = 0;
	return name_from_dns(buf, canon, name, family, &conf);
}
