static int nfs4_read_done_cb(struct rpc_task *task, struct nfs_pgio_header *hdr)
{
	struct nfs_server *server = NFS_SERVER(hdr->inode);

	trace_nfs4_read(hdr, task->tk_status);
	if (task->tk_status < 0) {
		struct nfs4_exception exception = {
			.inode = hdr->inode,
			.state = hdr->args.context->state,
			.stateid = &hdr->args.stateid,
		};
		task->tk_status = nfs4_async_handle_exception(task,
				server, task->tk_status, &exception);
		if (exception.retry) {
			rpc_restart_call_prepare(task);
			return -EAGAIN;
		}
	}

	if (task->tk_status > 0)
		renew_lease(server, hdr->timestamp);
	return 0;
}