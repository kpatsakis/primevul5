static unsigned int ipv6_defrag(void *priv,
				struct sk_buff *skb,
				const struct nf_hook_state *state)
{
	int err;

#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	/* Previously seen (loopback)?	*/
	if (skb->nfct && !nf_ct_is_template((struct nf_conn *)skb->nfct))
		return NF_ACCEPT;
#endif

	err = nf_ct_frag6_gather(state->net, skb,
				 nf_ct6_defrag_user(state->hook, skb));
	/* queued */
 	if (err == -EINPROGRESS)
 		return NF_STOLEN;
 
	return NF_ACCEPT;
 }
