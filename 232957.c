static inline struct ipv6_rt_hdr *ip6_rthdr_dup(struct ipv6_rt_hdr *src,
						gfp_t gfp)
{
	return src ? kmemdup(src, (src->hdrlen + 1) * 8, gfp) : NULL;
}