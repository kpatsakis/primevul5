int __lookup_name(struct address buf[static MAXADDRS], char canon[static 256], const char *name, int family, int flags)
{
	int cnt = 0, i, j;

	*canon = 0;
	if (name) {
		/* reject empty name and check len so it fits into temp bufs */
		size_t l = strnlen(name, 255);
		if (l-1 >= 254)
			return EAI_NONAME;
		memcpy(canon, name, l+1);
	}

	/* Procedurally, a request for v6 addresses with the v4-mapped
	 * flag set is like a request for unspecified family, followed
	 * by filtering of the results. */
	if (flags & AI_V4MAPPED) {
		if (family == AF_INET6) family = AF_UNSPEC;
		else flags -= AI_V4MAPPED;
	}

	/* Try each backend until there's at least one result. */
	cnt = name_from_null(buf, name, family, flags);
	if (!cnt) cnt = name_from_numeric(buf, name, family);
	if (!cnt && !(flags & AI_NUMERICHOST)) {
		cnt = name_from_hosts(buf, canon, name, family);
		if (!cnt) cnt = name_from_dns_search(buf, canon, name, family);
	}
	if (cnt<=0) return cnt ? cnt : EAI_NONAME;

	/* Filter/transform results for v4-mapped lookup, if requested. */
	if (flags & AI_V4MAPPED) {
		if (!(flags & AI_ALL)) {
			/* If any v6 results exist, remove v4 results. */
			for (i=0; i<cnt && buf[i].family != AF_INET6; i++);
			if (i<cnt) {
				for (j=0; i<cnt; i++) {
					if (buf[i].family == AF_INET6)
						buf[j++] = buf[i];
				}
				cnt = i = j;
			}
		}
		/* Translate any remaining v4 results to v6 */
		for (i=0; i<cnt; i++) {
			if (buf[i].family != AF_INET) continue;
			memcpy(buf[i].addr+12, buf[i].addr, 4);
			memcpy(buf[i].addr, "\0\0\0\0\0\0\0\0\0\0\xff\xff", 12);
			buf[i].family = AF_INET6;
		}
	}

	/* No further processing is needed if there are fewer than 2
	 * results or if there are only IPv4 results. */
	if (cnt<2 || family==AF_INET) return cnt;
	for (i=0; i<cnt; i++) if (buf[i].family != AF_INET) break;
	if (i==cnt) return cnt;

	int cs;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	/* The following implements a subset of RFC 3484/6724 destination
	 * address selection by generating a single 31-bit sort key for
	 * each address. Rules 3, 4, and 7 are omitted for having
	 * excessive runtime and code size cost and dubious benefit.
	 * So far the label/precedence table cannot be customized. */
	for (i=0; i<cnt; i++) {
		int key = 0;
		struct sockaddr_in6 sa, da = {
			.sin6_family = AF_INET6,
			.sin6_scope_id = buf[i].scopeid,
			.sin6_port = 65535
		};
		if (buf[i].family == AF_INET6) {
			memcpy(da.sin6_addr.s6_addr, buf[i].addr, 16);
		} else {
			memcpy(da.sin6_addr.s6_addr,
				"\0\0\0\0\0\0\0\0\0\0\xff\xff", 12);
			memcpy(da.sin6_addr.s6_addr+12, buf[i].addr, 4);
		}
		const struct policy *dpolicy = policyof(&da.sin6_addr);
		int dscope = scopeof(&da.sin6_addr);
		int dlabel = dpolicy->label;
		int dprec = dpolicy->prec;
		int prefixlen = 0;
		int fd = socket(AF_INET6, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
		if (fd >= 0) {
			if (!connect(fd, (void *)&da, sizeof da)) {
				key |= DAS_USABLE;
				if (!getsockname(fd, (void *)&sa,
				    &(socklen_t){sizeof sa})) {
					if (dscope == scopeof(&sa.sin6_addr))
						key |= DAS_MATCHINGSCOPE;
					if (dlabel == labelof(&sa.sin6_addr))
						key |= DAS_MATCHINGLABEL;
					prefixlen = prefixmatch(&sa.sin6_addr,
						&da.sin6_addr);
				}
			}
			close(fd);
		}
		key |= dprec << DAS_PREC_SHIFT;
		key |= (15-dscope) << DAS_SCOPE_SHIFT;
		key |= prefixlen << DAS_PREFIX_SHIFT;
		key |= (MAXADDRS-i) << DAS_ORDER_SHIFT;
		buf[i].sortkey = key;
	}
	qsort(buf, cnt, sizeof *buf, addrcmp);

	pthread_setcancelstate(cs, 0);

	return cnt;
}
