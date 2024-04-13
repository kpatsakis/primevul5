static void nfs4_exchange_id_release(void *data)
{
	struct nfs41_exchange_id_data *cdata =
					(struct nfs41_exchange_id_data *)data;

	nfs_put_client(cdata->args.client);
	kfree(cdata->res.impl_id);
	kfree(cdata->res.server_scope);
	kfree(cdata->res.server_owner);
	kfree(cdata);
}