static int nfs4_proc_sequence(struct nfs_client *clp, const struct cred *cred)
{
	struct rpc_task *task;
	int ret;

	task = _nfs41_proc_sequence(clp, cred, NULL, true);
	if (IS_ERR(task)) {
		ret = PTR_ERR(task);
		goto out;
	}
	ret = rpc_wait_for_completion_task(task);
	if (!ret)
		ret = task->tk_status;
	rpc_put_task(task);
out:
	dprintk("<-- %s status=%d\n", __func__, ret);
	return ret;
}