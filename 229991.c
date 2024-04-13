nfs4_map_atomic_open_share(struct nfs_server *server,
		fmode_t fmode, int openflags)
{
	u32 res = 0;

	switch (fmode & (FMODE_READ | FMODE_WRITE)) {
	case FMODE_READ:
		res = NFS4_SHARE_ACCESS_READ;
		break;
	case FMODE_WRITE:
		res = NFS4_SHARE_ACCESS_WRITE;
		break;
	case FMODE_READ|FMODE_WRITE:
		res = NFS4_SHARE_ACCESS_BOTH;
	}
	if (!(server->caps & NFS_CAP_ATOMIC_OPEN_V1))
		goto out;
	/* Want no delegation if we're using O_DIRECT */
	if (openflags & O_DIRECT)
		res |= NFS4_SHARE_WANT_NO_DELEG;
out:
	return res;
}