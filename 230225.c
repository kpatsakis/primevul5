static int nfs4_call_sync_custom(struct rpc_task_setup *task_setup)
{
	int ret;
	struct rpc_task *task;

	task = rpc_run_task(task_setup);
	if (IS_ERR(task))
		return PTR_ERR(task);

	ret = task->tk_status;
	rpc_put_task(task);
	return ret;
}