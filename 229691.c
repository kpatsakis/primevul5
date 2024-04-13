nfs4_async_handle_error(struct rpc_task *task, struct nfs_server *server,
			struct nfs4_state *state, long *timeout)
{
	struct nfs4_exception exception = {
		.state = state,
	};

	if (task->tk_status >= 0)
		return 0;
	if (timeout)
		exception.timeout = *timeout;
	task->tk_status = nfs4_async_handle_exception(task, server,
			task->tk_status,
			&exception);
	if (exception.delay && timeout)
		*timeout = exception.timeout;
	if (exception.retry)
		return -EAGAIN;
	return 0;
}