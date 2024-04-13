nfs4_layoutget_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutget *lgp = calldata;
	struct nfs_server *server = NFS_SERVER(lgp->args.inode);

	dprintk("--> %s\n", __func__);
	nfs4_setup_sequence(server->nfs_client, &lgp->args.seq_args,
				&lgp->res.seq_res, task);
	dprintk("<-- %s\n", __func__);
}