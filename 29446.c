static int scopeof(const struct in6_addr *a)
{
	if (IN6_IS_ADDR_MULTICAST(a)) return a->s6_addr[1] & 15;
	if (IN6_IS_ADDR_LINKLOCAL(a)) return 2;
	if (IN6_IS_ADDR_LOOPBACK(a)) return 2;
	if (IN6_IS_ADDR_SITELOCAL(a)) return 5;
	return 14;
}
