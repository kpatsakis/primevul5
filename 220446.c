static int nf_tables_updobj(const struct nft_ctx *ctx,
			    const struct nft_object_type *type,
			    const struct nlattr *attr,
			    struct nft_object *obj)
{
	struct nft_object *newobj;
	struct nft_trans *trans;
	int err;

	trans = nft_trans_alloc(ctx, NFT_MSG_NEWOBJ,
				sizeof(struct nft_trans_obj));
	if (!trans)
		return -ENOMEM;

	newobj = nft_obj_init(ctx, type, attr);
	if (IS_ERR(newobj)) {
		err = PTR_ERR(newobj);
		goto err_free_trans;
	}

	nft_trans_obj(trans) = obj;
	nft_trans_obj_update(trans) = true;
	nft_trans_obj_newobj(trans) = newobj;
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;

err_free_trans:
	kfree(trans);
	return err;
}