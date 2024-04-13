static int nf_tables_newflowtable(struct sk_buff *skb,
				  const struct nfnl_info *info,
				  const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	struct nft_flowtable_hook flowtable_hook;
	u8 genmask = nft_genmask_next(info->net);
	const struct nf_flowtable_type *type;
	int family = nfmsg->nfgen_family;
	struct nft_flowtable *flowtable;
	struct nft_hook *hook, *next;
	struct net *net = info->net;
	struct nft_table *table;
	struct nft_ctx ctx;
	int err;

	if (!nla[NFTA_FLOWTABLE_TABLE] ||
	    !nla[NFTA_FLOWTABLE_NAME] ||
	    !nla[NFTA_FLOWTABLE_HOOK])
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_FLOWTABLE_TABLE], family,
				 genmask, NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_FLOWTABLE_TABLE]);
		return PTR_ERR(table);
	}

	flowtable = nft_flowtable_lookup(table, nla[NFTA_FLOWTABLE_NAME],
					 genmask);
	if (IS_ERR(flowtable)) {
		err = PTR_ERR(flowtable);
		if (err != -ENOENT) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_FLOWTABLE_NAME]);
			return err;
		}
	} else {
		if (info->nlh->nlmsg_flags & NLM_F_EXCL) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_FLOWTABLE_NAME]);
			return -EEXIST;
		}

		nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

		return nft_flowtable_update(&ctx, info->nlh, flowtable);
	}

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

	flowtable = kzalloc(sizeof(*flowtable), GFP_KERNEL);
	if (!flowtable)
		return -ENOMEM;

	flowtable->table = table;
	flowtable->handle = nf_tables_alloc_handle(table);
	INIT_LIST_HEAD(&flowtable->hook_list);

	flowtable->name = nla_strdup(nla[NFTA_FLOWTABLE_NAME], GFP_KERNEL);
	if (!flowtable->name) {
		err = -ENOMEM;
		goto err1;
	}

	type = nft_flowtable_type_get(net, family);
	if (IS_ERR(type)) {
		err = PTR_ERR(type);
		goto err2;
	}

	if (nla[NFTA_FLOWTABLE_FLAGS]) {
		flowtable->data.flags =
			ntohl(nla_get_be32(nla[NFTA_FLOWTABLE_FLAGS]));
		if (flowtable->data.flags & ~NFT_FLOWTABLE_MASK) {
			err = -EOPNOTSUPP;
			goto err3;
		}
	}

	write_pnet(&flowtable->data.net, net);
	flowtable->data.type = type;
	err = type->init(&flowtable->data);
	if (err < 0)
		goto err3;

	err = nft_flowtable_parse_hook(&ctx, nla[NFTA_FLOWTABLE_HOOK],
				       &flowtable_hook, flowtable, true);
	if (err < 0)
		goto err4;

	list_splice(&flowtable_hook.list, &flowtable->hook_list);
	flowtable->data.priority = flowtable_hook.priority;
	flowtable->hooknum = flowtable_hook.num;

	err = nft_register_flowtable_net_hooks(ctx.net, table,
					       &flowtable->hook_list,
					       flowtable);
	if (err < 0) {
		nft_flowtable_hooks_destroy(&flowtable->hook_list);
		goto err4;
	}

	err = nft_trans_flowtable_add(&ctx, NFT_MSG_NEWFLOWTABLE, flowtable);
	if (err < 0)
		goto err5;

	list_add_tail_rcu(&flowtable->list, &table->flowtables);
	table->use++;

	return 0;
err5:
	list_for_each_entry_safe(hook, next, &flowtable->hook_list, list) {
		nft_unregister_flowtable_hook(net, flowtable, hook);
		list_del_rcu(&hook->list);
		kfree_rcu(hook, rcu);
	}
err4:
	flowtable->data.type->free(&flowtable->data);
err3:
	module_put(type->owner);
err2:
	kfree(flowtable->name);
err1:
	kfree(flowtable);
	return err;
}