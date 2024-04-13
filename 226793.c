static struct sctp_af *sctp_sockaddr_af(struct sctp_sock *opt,
					union sctp_addr *addr, int len)
{
	struct sctp_af *af;

	/* Check minimum size.  */
	if (len < sizeof (struct sockaddr))
		return NULL;

	if (!opt->pf->af_supported(addr->sa.sa_family, opt))
		return NULL;

	if (addr->sa.sa_family == AF_INET6) {
		if (len < SIN6_LEN_RFC2133)
			return NULL;
		/* V4 mapped address are really of AF_INET family */
		if (ipv6_addr_v4mapped(&addr->v6.sin6_addr) &&
		    !opt->pf->af_supported(AF_INET, opt))
			return NULL;
	}

	/* If we get this far, af is valid. */
	af = sctp_get_af_specific(addr->sa.sa_family);

	if (len < af->sockaddr_len)
		return NULL;

	return af;
}