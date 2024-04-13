static struct nfs4_state *nfs4_do_open(struct inode *dir,
					struct nfs_open_context *ctx,
					int flags,
					struct iattr *sattr,
					struct nfs4_label *label,
					int *opened)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	struct nfs4_state *res;
	struct nfs4_open_createattrs c = {
		.label = label,
		.sattr = sattr,
		.verf = {
			[0] = (__u32)jiffies,
			[1] = (__u32)current->pid,
		},
	};
	int status;

	do {
		status = _nfs4_do_open(dir, ctx, flags, &c, opened);
		res = ctx->state;
		trace_nfs4_open_file(ctx, flags, status);
		if (status == 0)
			break;
		/* NOTE: BAD_SEQID means the server and client disagree about the
		 * book-keeping w.r.t. state-changing operations
		 * (OPEN/CLOSE/LOCK/LOCKU...)
		 * It is actually a sign of a bug on the client or on the server.
		 *
		 * If we receive a BAD_SEQID error in the particular case of
		 * doing an OPEN, we assume that nfs_increment_open_seqid() will
		 * have unhashed the old state_owner for us, and that we can
		 * therefore safely retry using a new one. We should still warn
		 * the user though...
		 */
		if (status == -NFS4ERR_BAD_SEQID) {
			pr_warn_ratelimited("NFS: v4 server %s "
					" returned a bad sequence-id error!\n",
					NFS_SERVER(dir)->nfs_client->cl_hostname);
			exception.retry = 1;
			continue;
		}
		/*
		 * BAD_STATEID on OPEN means that the server cancelled our
		 * state before it received the OPEN_CONFIRM.
		 * Recover by retrying the request as per the discussion
		 * on Page 181 of RFC3530.
		 */
		if (status == -NFS4ERR_BAD_STATEID) {
			exception.retry = 1;
			continue;
		}
		if (status == -NFS4ERR_EXPIRED) {
			nfs4_schedule_lease_recovery(server->nfs_client);
			exception.retry = 1;
			continue;
		}
		if (status == -EAGAIN) {
			/* We must have found a delegation */
			exception.retry = 1;
			continue;
		}
		if (nfs4_clear_cap_atomic_open_v1(server, status, &exception))
			continue;
		res = ERR_PTR(nfs4_handle_exception(server,
					status, &exception));
	} while (exception.retry);
	return res;
}