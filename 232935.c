static void ip_copy_addrs(struct iphdr *iph, const struct flowi4 *fl4)
{
	BUILD_BUG_ON(offsetof(typeof(*fl4), daddr) !=
		     offsetof(typeof(*fl4), saddr) + sizeof(fl4->saddr));
	memcpy(&iph->saddr, &fl4->saddr,
	       sizeof(fl4->saddr) + sizeof(fl4->daddr));
}