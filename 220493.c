static void nf_tables_rule_notify(const struct nft_ctx *ctx,
				  const struct nft_rule *rule, int event)
{
	struct nftables_pernet *nft_net = nft_pernet(ctx->net);
	struct sk_buff *skb;
	int err;

	if (!ctx->report &&
	    !nfnetlink_has_listeners(ctx->net, NFNLGRP_NFTABLES))
		return;

	skb = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		goto err;

	err = nf_tables_fill_rule_info(skb, ctx->net, ctx->portid, ctx->seq,
				       event, 0, ctx->family, ctx->table,
				       ctx->chain, rule, NULL);
	if (err < 0) {
		kfree_skb(skb);
		goto err;
	}

	nft_notify_enqueue(skb, ctx->report, &nft_net->notify_list);
	return;
err:
	nfnetlink_set_err(ctx->net, ctx->portid, NFNLGRP_NFTABLES, -ENOBUFS);
}