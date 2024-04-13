static inline struct ipv6_opt_hdr *ip6_opt_dup(struct ipv6_opt_hdr *src,
					       gfp_t gfp)
{
	return src ? kmemdup(src, (src->hdrlen + 1) * 8, gfp) : NULL;
}