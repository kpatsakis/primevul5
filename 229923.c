static int nfs4_proc_get_root(struct nfs_server *server, struct nfs_fh *mntfh,
			      struct nfs_fsinfo *info)
{
	int error;
	struct nfs_fattr *fattr = info->fattr;
	struct nfs4_label *label = fattr->label;

	error = nfs4_server_capabilities(server, mntfh);
	if (error < 0) {
		dprintk("nfs4_get_root: getcaps error = %d\n", -error);
		return error;
	}

	error = nfs4_proc_getattr(server, mntfh, fattr, label, NULL);
	if (error < 0) {
		dprintk("nfs4_get_root: getattr error = %d\n", -error);
		goto out;
	}

	if (fattr->valid & NFS_ATTR_FATTR_FSID &&
	    !nfs_fsid_equal(&server->fsid, &fattr->fsid))
		memcpy(&server->fsid, &fattr->fsid, sizeof(server->fsid));

out:
	return error;
}