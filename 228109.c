static inline struct ip_options_rcu *tcp_v4_save_options(struct sk_buff *skb)
{
	const struct ip_options *opt = &TCP_SKB_CB(skb)->header.h4.opt;
	struct ip_options_rcu *dopt = NULL;

	if (opt->optlen) {
		int opt_size = sizeof(*dopt) + opt->optlen;

		dopt = kmalloc(opt_size, GFP_ATOMIC);
		if (dopt && __ip_options_echo(&dopt->opt, skb, opt)) {
			kfree(dopt);
			dopt = NULL;
		}
	}
	return dopt;
}