static void nfs4_proc_write_setup(struct nfs_pgio_header *hdr,
				  struct rpc_message *msg,
				  struct rpc_clnt **clnt)
{
	struct nfs_server *server = NFS_SERVER(hdr->inode);

	if (!nfs4_write_need_cache_consistency_data(hdr)) {
		hdr->args.bitmask = NULL;
		hdr->res.fattr = NULL;
	} else
		hdr->args.bitmask = server->cache_consistency_bitmask;

	if (!hdr->pgio_done_cb)
		hdr->pgio_done_cb = nfs4_write_done_cb;
	hdr->res.server = server;
	hdr->timestamp   = jiffies;

	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_WRITE];
	nfs4_init_sequence(&hdr->args.seq_args, &hdr->res.seq_res, 0, 0);
	nfs4_state_protect_write(server->nfs_client, clnt, msg, hdr);
}