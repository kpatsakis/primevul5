nfs4_layoutreturn_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;

	dprintk("--> %s\n", __func__);
	nfs4_setup_sequence(lrp->clp,
			&lrp->args.seq_args,
			&lrp->res.seq_res,
			task);
	if (!pnfs_layout_is_valid(lrp->args.layout))
		rpc_exit(task, 0);
}