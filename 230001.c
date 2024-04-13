nfs4_layoutget_handle_exception(struct rpc_task *task,
		struct nfs4_layoutget *lgp, struct nfs4_exception *exception)
{
	struct inode *inode = lgp->args.inode;
	struct nfs_server *server = NFS_SERVER(inode);
	struct pnfs_layout_hdr *lo;
	int nfs4err = task->tk_status;
	int err, status = 0;
	LIST_HEAD(head);

	dprintk("--> %s tk_status => %d\n", __func__, -task->tk_status);

	nfs4_sequence_free_slot(&lgp->res.seq_res);

	switch (nfs4err) {
	case 0:
		goto out;

	/*
	 * NFS4ERR_LAYOUTUNAVAILABLE means we are not supposed to use pnfs
	 * on the file. set tk_status to -ENODATA to tell upper layer to
	 * retry go inband.
	 */
	case -NFS4ERR_LAYOUTUNAVAILABLE:
		status = -ENODATA;
		goto out;
	/*
	 * NFS4ERR_BADLAYOUT means the MDS cannot return a layout of
	 * length lgp->args.minlength != 0 (see RFC5661 section 18.43.3).
	 */
	case -NFS4ERR_BADLAYOUT:
		status = -EOVERFLOW;
		goto out;
	/*
	 * NFS4ERR_LAYOUTTRYLATER is a conflict with another client
	 * (or clients) writing to the same RAID stripe except when
	 * the minlength argument is 0 (see RFC5661 section 18.43.3).
	 *
	 * Treat it like we would RECALLCONFLICT -- we retry for a little
	 * while, and then eventually give up.
	 */
	case -NFS4ERR_LAYOUTTRYLATER:
		if (lgp->args.minlength == 0) {
			status = -EOVERFLOW;
			goto out;
		}
		status = -EBUSY;
		break;
	case -NFS4ERR_RECALLCONFLICT:
		status = -ERECALLCONFLICT;
		break;
	case -NFS4ERR_DELEG_REVOKED:
	case -NFS4ERR_ADMIN_REVOKED:
	case -NFS4ERR_EXPIRED:
	case -NFS4ERR_BAD_STATEID:
		exception->timeout = 0;
		spin_lock(&inode->i_lock);
		lo = NFS_I(inode)->layout;
		/* If the open stateid was bad, then recover it. */
		if (!lo || test_bit(NFS_LAYOUT_INVALID_STID, &lo->plh_flags) ||
		    !nfs4_stateid_match_other(&lgp->args.stateid, &lo->plh_stateid)) {
			spin_unlock(&inode->i_lock);
			exception->state = lgp->args.ctx->state;
			exception->stateid = &lgp->args.stateid;
			break;
		}

		/*
		 * Mark the bad layout state as invalid, then retry
		 */
		pnfs_mark_layout_stateid_invalid(lo, &head);
		spin_unlock(&inode->i_lock);
		nfs_commit_inode(inode, 0);
		pnfs_free_lseg_list(&head);
		status = -EAGAIN;
		goto out;
	}

	err = nfs4_handle_exception(server, nfs4err, exception);
	if (!status) {
		if (exception->retry)
			status = -EAGAIN;
		else
			status = err;
	}
out:
	dprintk("<-- %s\n", __func__);
	return status;
}