static void nfs4_layoutcommit_release(void *calldata)
{
	struct nfs4_layoutcommit_data *data = calldata;

	pnfs_cleanup_layoutcommit(data);
	nfs_post_op_update_inode_force_wcc(data->args.inode,
					   data->res.fattr);
	put_cred(data->cred);
	nfs_iput_and_deactive(data->inode);
	kfree(data);
}