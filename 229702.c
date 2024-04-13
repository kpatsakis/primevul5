static int nfs4_handle_delegation_recall_error(struct nfs_server *server, struct nfs4_state *state, const nfs4_stateid *stateid, struct file_lock *fl, int err)
{
	switch (err) {
		default:
			printk(KERN_ERR "NFS: %s: unhandled error "
					"%d.\n", __func__, err);
		case 0:
		case -ENOENT:
		case -EAGAIN:
		case -ESTALE:
		case -ETIMEDOUT:
			break;
		case -NFS4ERR_BADSESSION:
		case -NFS4ERR_BADSLOT:
		case -NFS4ERR_BAD_HIGH_SLOT:
		case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
		case -NFS4ERR_DEADSESSION:
			return -EAGAIN;
		case -NFS4ERR_STALE_CLIENTID:
		case -NFS4ERR_STALE_STATEID:
			/* Don't recall a delegation if it was lost */
			nfs4_schedule_lease_recovery(server->nfs_client);
			return -EAGAIN;
		case -NFS4ERR_MOVED:
			nfs4_schedule_migration_recovery(server);
			return -EAGAIN;
		case -NFS4ERR_LEASE_MOVED:
			nfs4_schedule_lease_moved_recovery(server->nfs_client);
			return -EAGAIN;
		case -NFS4ERR_DELEG_REVOKED:
		case -NFS4ERR_ADMIN_REVOKED:
		case -NFS4ERR_EXPIRED:
		case -NFS4ERR_BAD_STATEID:
		case -NFS4ERR_OPENMODE:
			nfs_inode_find_state_and_recover(state->inode,
					stateid);
			nfs4_schedule_stateid_recovery(server, state);
			return -EAGAIN;
		case -NFS4ERR_DELAY:
		case -NFS4ERR_GRACE:
			ssleep(1);
			return -EAGAIN;
		case -ENOMEM:
		case -NFS4ERR_DENIED:
			if (fl) {
				struct nfs4_lock_state *lsp = fl->fl_u.nfs4_fl.owner;
				if (lsp)
					set_bit(NFS_LOCK_LOST, &lsp->ls_flags);
			}
			return 0;
	}
	return err;
}