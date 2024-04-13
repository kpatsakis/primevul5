static bool nfs4_write_stateid_changed(struct rpc_task *task,
		struct nfs_pgio_args *args)
{

	if (!nfs4_error_stateid_expired(task->tk_status) ||
		nfs4_stateid_is_current(&args->stateid,
				args->context,
				args->lock_context,
				FMODE_WRITE))
		return false;
	rpc_restart_call_prepare(task);
	return true;
}