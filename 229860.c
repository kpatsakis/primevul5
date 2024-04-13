static int nfs41_proc_async_sequence(struct nfs_client *clp, const struct cred *cred, unsigned renew_flags)
{
	struct rpc_task *task;
	int ret = 0;

	if ((renew_flags & NFS4_RENEW_TIMEOUT) == 0)
		return -EAGAIN;
	task = _nfs41_proc_sequence(clp, cred, NULL, false);
	if (IS_ERR(task))
		ret = PTR_ERR(task);
	else
		rpc_put_task_async(task);
	dprintk("<-- %s status=%d\n", __func__, ret);
	return ret;
}