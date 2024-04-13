static void nfs4_delegreturn_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_delegreturndata *d_data;
	struct pnfs_layout_hdr *lo;

	d_data = (struct nfs4_delegreturndata *)data;

	if (!d_data->lr.roc && nfs4_wait_on_layoutreturn(d_data->inode, task)) {
		nfs4_sequence_done(task, &d_data->res.seq_res);
		return;
	}

	lo = d_data->args.lr_args ? d_data->args.lr_args->layout : NULL;
	if (lo && !pnfs_layout_is_valid(lo)) {
		d_data->args.lr_args = NULL;
		d_data->res.lr_res = NULL;
	}

	nfs4_setup_sequence(d_data->res.server->nfs_client,
			&d_data->args.seq_args,
			&d_data->res.seq_res,
			task);
}