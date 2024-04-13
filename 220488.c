static int nf_tables_newobj(struct sk_buff *skb, const struct nfnl_info *info,
			    const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	const struct nft_object_type *type;
	int family = nfmsg->nfgen_family;
	struct net *net = info->net;
	struct nft_table *table;
	struct nft_object *obj;
	struct nft_ctx ctx;
	u32 objtype;
	int err;

	if (!nla[NFTA_OBJ_TYPE] ||
	    !nla[NFTA_OBJ_NAME] ||
	    !nla[NFTA_OBJ_DATA])
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_OBJ_TABLE], family, genmask,
				 NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_OBJ_TABLE]);
		return PTR_ERR(table);
	}

	objtype = ntohl(nla_get_be32(nla[NFTA_OBJ_TYPE]));
	obj = nft_obj_lookup(net, table, nla[NFTA_OBJ_NAME], objtype, genmask);
	if (IS_ERR(obj)) {
		err = PTR_ERR(obj);
		if (err != -ENOENT) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_OBJ_NAME]);
			return err;
		}
	} else {
		if (info->nlh->nlmsg_flags & NLM_F_EXCL) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_OBJ_NAME]);
			return -EEXIST;
		}
		if (info->nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EOPNOTSUPP;

		type = __nft_obj_type_get(objtype);
		nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

		return nf_tables_updobj(&ctx, type, nla[NFTA_OBJ_DATA], obj);
	}

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

	type = nft_obj_type_get(net, objtype);
	if (IS_ERR(type))
		return PTR_ERR(type);

	obj = nft_obj_init(&ctx, type, nla[NFTA_OBJ_DATA]);
	if (IS_ERR(obj)) {
		err = PTR_ERR(obj);
		goto err_init;
	}
	obj->key.table = table;
	obj->handle = nf_tables_alloc_handle(table);

	obj->key.name = nla_strdup(nla[NFTA_OBJ_NAME], GFP_KERNEL);
	if (!obj->key.name) {
		err = -ENOMEM;
		goto err_strdup;
	}

	if (nla[NFTA_OBJ_USERDATA]) {
		obj->udata = nla_memdup(nla[NFTA_OBJ_USERDATA], GFP_KERNEL);
		if (obj->udata == NULL)
			goto err_userdata;

		obj->udlen = nla_len(nla[NFTA_OBJ_USERDATA]);
	}

	err = nft_trans_obj_add(&ctx, NFT_MSG_NEWOBJ, obj);
	if (err < 0)
		goto err_trans;

	err = rhltable_insert(&nft_objname_ht, &obj->rhlhead,
			      nft_objname_ht_params);
	if (err < 0)
		goto err_obj_ht;

	list_add_tail_rcu(&obj->list, &table->objects);
	table->use++;
	return 0;
err_obj_ht:
	/* queued in transaction log */
	INIT_LIST_HEAD(&obj->list);
	return err;
err_trans:
	kfree(obj->udata);
err_userdata:
	kfree(obj->key.name);
err_strdup:
	if (obj->ops->destroy)
		obj->ops->destroy(&ctx, obj);
	kfree(obj);
err_init:
	module_put(type->owner);
	return err;
}