static u32 __ipv6_select_ident(struct net *net,
			       const struct in6_addr *dst,
			       const struct in6_addr *src)
{
	u32 id;

	do {
		id = prandom_u32();
	} while (!id);

	return id;
}