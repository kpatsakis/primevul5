static int nfs4_do_setattr(struct inode *inode, const struct cred *cred,
			   struct nfs_fattr *fattr, struct iattr *sattr,
			   struct nfs_open_context *ctx, struct nfs4_label *ilabel,
			   struct nfs4_label *olabel)
{
	struct nfs_server *server = NFS_SERVER(inode);
	__u32 bitmask[NFS4_BITMASK_SZ];
	struct nfs4_state *state = ctx ? ctx->state : NULL;
	struct nfs_setattrargs	arg = {
		.fh		= NFS_FH(inode),
		.iap		= sattr,
		.server		= server,
		.bitmask = bitmask,
		.label		= ilabel,
	};
	struct nfs_setattrres  res = {
		.fattr		= fattr,
		.label		= olabel,
		.server		= server,
	};
	struct nfs4_exception exception = {
		.state = state,
		.inode = inode,
		.stateid = &arg.stateid,
	};
	int err;

	do {
		nfs4_bitmap_copy_adjust_setattr(bitmask,
				nfs4_bitmask(server, olabel),
				inode);

		err = _nfs4_do_setattr(inode, &arg, &res, cred, ctx);
		switch (err) {
		case -NFS4ERR_OPENMODE:
			if (!(sattr->ia_valid & ATTR_SIZE)) {
				pr_warn_once("NFSv4: server %s is incorrectly "
						"applying open mode checks to "
						"a SETATTR that is not "
						"changing file size.\n",
						server->nfs_client->cl_hostname);
			}
			if (state && !(state->state & FMODE_WRITE)) {
				err = -EBADF;
				if (sattr->ia_valid & ATTR_OPEN)
					err = -EACCES;
				goto out;
			}
		}
		err = nfs4_handle_exception(server, err, &exception);
	} while (exception.retry);
out:
	return err;
}