static void nft_obj_commit_update(struct nft_trans *trans)
{
	struct nft_object *newobj;
	struct nft_object *obj;

	obj = nft_trans_obj(trans);
	newobj = nft_trans_obj_newobj(trans);

	if (obj->ops->update)
		obj->ops->update(obj, newobj);

	kfree(newobj);
}