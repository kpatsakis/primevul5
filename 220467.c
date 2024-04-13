static int nf_tables_getobj(struct sk_buff *skb, const struct nfnl_info *info,
			    const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_cur(info->net);
	int family = nfmsg->nfgen_family;
	const struct nft_table *table;
	struct net *net = info->net;
	struct nft_object *obj;
	struct sk_buff *skb2;
	bool reset = false;
	u32 objtype;
	int err;

	if (info->nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.start = nf_tables_dump_obj_start,
			.dump = nf_tables_dump_obj,
			.done = nf_tables_dump_obj_done,
			.module = THIS_MODULE,
			.data = (void *)nla,
		};

		return nft_netlink_dump_start_rcu(info->sk, skb, info->nlh, &c);
	}

	if (!nla[NFTA_OBJ_NAME] ||
	    !nla[NFTA_OBJ_TYPE])
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_OBJ_TABLE], family, genmask, 0);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_OBJ_TABLE]);
		return PTR_ERR(table);
	}

	objtype = ntohl(nla_get_be32(nla[NFTA_OBJ_TYPE]));
	obj = nft_obj_lookup(net, table, nla[NFTA_OBJ_NAME], objtype, genmask);
	if (IS_ERR(obj)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_OBJ_NAME]);
		return PTR_ERR(obj);
	}

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!skb2)
		return -ENOMEM;

	if (NFNL_MSG_TYPE(info->nlh->nlmsg_type) == NFT_MSG_GETOBJ_RESET)
		reset = true;

	if (reset) {
		const struct nftables_pernet *nft_net;
		char *buf;

		nft_net = nft_pernet(net);
		buf = kasprintf(GFP_ATOMIC, "%s:%u", table->name, nft_net->base_seq);

		audit_log_nfcfg(buf,
				family,
				obj->handle,
				AUDIT_NFT_OP_OBJ_RESET,
				GFP_ATOMIC);
		kfree(buf);
	}

	err = nf_tables_fill_obj_info(skb2, net, NETLINK_CB(skb).portid,
				      info->nlh->nlmsg_seq, NFT_MSG_NEWOBJ, 0,
				      family, table, obj, reset);
	if (err < 0)
		goto err_fill_obj_info;

	return nfnetlink_unicast(skb2, net, NETLINK_CB(skb).portid);

err_fill_obj_info:
	kfree_skb(skb2);
	return err;
}