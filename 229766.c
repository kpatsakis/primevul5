static bool nfs4_clear_cap_atomic_open_v1(struct nfs_server *server,
		int err, struct nfs4_exception *exception)
{
	if (err != -EINVAL)
		return false;
	if (!(server->caps & NFS_CAP_ATOMIC_OPEN_V1))
		return false;
	server->caps &= ~NFS_CAP_ATOMIC_OPEN_V1;
	exception->retry = 1;
	return true;
}