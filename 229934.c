static void nfs4_proc_read_setup(struct nfs_pgio_header *hdr,
				 struct rpc_message *msg)
{
	hdr->timestamp   = jiffies;
	if (!hdr->pgio_done_cb)
		hdr->pgio_done_cb = nfs4_read_done_cb;
	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_READ];
	nfs4_init_sequence(&hdr->args.seq_args, &hdr->res.seq_res, 0, 0);
}