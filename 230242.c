static int nfs4_proc_pgio_rpc_prepare(struct rpc_task *task,
				      struct nfs_pgio_header *hdr)
{
	if (nfs4_setup_sequence(NFS_SERVER(hdr->inode)->nfs_client,
			&hdr->args.seq_args,
			&hdr->res.seq_res,
			task))
		return 0;
	if (nfs4_set_rw_stateid(&hdr->args.stateid, hdr->args.context,
				hdr->args.lock_context,
				hdr->rw_mode) == -EIO)
		return -EIO;
	if (unlikely(test_bit(NFS_CONTEXT_BAD, &hdr->args.context->flags)))
		return -EIO;
	return 0;
}