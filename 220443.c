void nft_obj_notify(struct net *net, const struct nft_table *table,
		    struct nft_object *obj, u32 portid, u32 seq, int event,
		    int family, int report, gfp_t gfp)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct sk_buff *skb;
	int err;
	char *buf = kasprintf(gfp, "%s:%u",
			      table->name, nft_net->base_seq);

	audit_log_nfcfg(buf,
			family,
			obj->handle,
			event == NFT_MSG_NEWOBJ ?
				 AUDIT_NFT_OP_OBJ_REGISTER :
				 AUDIT_NFT_OP_OBJ_UNREGISTER,
			gfp);
	kfree(buf);

	if (!report &&
	    !nfnetlink_has_listeners(net, NFNLGRP_NFTABLES))
		return;

	skb = nlmsg_new(NLMSG_GOODSIZE, gfp);
	if (skb == NULL)
		goto err;

	err = nf_tables_fill_obj_info(skb, net, portid, seq, event, 0, family,
				      table, obj, false);
	if (err < 0) {
		kfree_skb(skb);
		goto err;
	}

	nft_notify_enqueue(skb, report, &nft_net->notify_list);
	return;
err:
	nfnetlink_set_err(net, portid, NFNLGRP_NFTABLES, -ENOBUFS);
}