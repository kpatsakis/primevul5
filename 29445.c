static int prefixmatch(const struct in6_addr *s, const struct in6_addr *d)
{
	/* FIXME: The common prefix length should be limited to no greater
	 * than the nominal length of the prefix portion of the source
	 * address. However the definition of the source prefix length is
	 * not clear and thus this limiting is not yet implemented. */
	unsigned i;
	for (i=0; i<128 && !((s->s6_addr[i/8]^d->s6_addr[i/8])&(128>>(i%8))); i++);
	return i;
}
