static int nfs4_do_handle_exception(struct nfs_server *server,
		int errorcode, struct nfs4_exception *exception)
{
	struct nfs_client *clp = server->nfs_client;
	struct nfs4_state *state = exception->state;
	const nfs4_stateid *stateid;
	struct inode *inode = exception->inode;
	int ret = errorcode;

	exception->delay = 0;
	exception->recovering = 0;
	exception->retry = 0;

	stateid = nfs4_recoverable_stateid(exception->stateid);
	if (stateid == NULL && state != NULL)
		stateid = nfs4_recoverable_stateid(&state->stateid);

	switch(errorcode) {
		case 0:
			return 0;
		case -NFS4ERR_BADHANDLE:
		case -ESTALE:
			if (inode != NULL && S_ISREG(inode->i_mode))
				pnfs_destroy_layout(NFS_I(inode));
			break;
		case -NFS4ERR_DELEG_REVOKED:
		case -NFS4ERR_ADMIN_REVOKED:
		case -NFS4ERR_EXPIRED:
		case -NFS4ERR_BAD_STATEID:
		case -NFS4ERR_PARTNER_NO_AUTH:
			if (inode != NULL && stateid != NULL) {
				nfs_inode_find_state_and_recover(inode,
						stateid);
				goto wait_on_recovery;
			}
			/* Fall through */
		case -NFS4ERR_OPENMODE:
			if (inode) {
				int err;

				err = nfs_async_inode_return_delegation(inode,
						stateid);
				if (err == 0)
					goto wait_on_recovery;
				if (stateid != NULL && stateid->type == NFS4_DELEGATION_STATEID_TYPE) {
					exception->retry = 1;
					break;
				}
			}
			if (state == NULL)
				break;
			ret = nfs4_schedule_stateid_recovery(server, state);
			if (ret < 0)
				break;
			goto wait_on_recovery;
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_STALE_CLIENTID:
			nfs4_schedule_lease_recovery(clp);
			goto wait_on_recovery;
		case -NFS4ERR_MOVED:
			ret = nfs4_schedule_migration_recovery(server);
			if (ret < 0)
				break;
			goto wait_on_recovery;
		case -NFS4ERR_LEASE_MOVED:
			nfs4_schedule_lease_moved_recovery(clp);
			goto wait_on_recovery;
#if defined(CONFIG_NFS_V4_1)
		case -NFS4ERR_BADSESSION:
		case -NFS4ERR_BADSLOT:
		case -NFS4ERR_BAD_HIGH_SLOT:
		case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
		case -NFS4ERR_DEADSESSION:
		case -NFS4ERR_SEQ_FALSE_RETRY:
		case -NFS4ERR_SEQ_MISORDERED:
			/* Handled in nfs41_sequence_process() */
			goto wait_on_recovery;
#endif /* defined(CONFIG_NFS_V4_1) */
		case -NFS4ERR_FILE_OPEN:
			if (exception->timeout > HZ) {
				/* We have retried a decent amount, time to
				 * fail
				 */
				ret = -EBUSY;
				break;
			}
			/* Fall through */
		case -NFS4ERR_DELAY:
			nfs_inc_server_stats(server, NFSIOS_DELAY);
			/* Fall through */
		case -NFS4ERR_GRACE:
		case -NFS4ERR_LAYOUTTRYLATER:
		case -NFS4ERR_RECALLCONFLICT:
			exception->delay = 1;
			return 0;

		case -NFS4ERR_RETRY_UNCACHED_REP:
		case -NFS4ERR_OLD_STATEID:
			exception->retry = 1;
			break;
		case -NFS4ERR_BADOWNER:
			/* The following works around a Linux server bug! */
		case -NFS4ERR_BADNAME:
			if (server->caps & NFS_CAP_UIDGID_NOMAP) {
				server->caps &= ~NFS_CAP_UIDGID_NOMAP;
				exception->retry = 1;
				printk(KERN_WARNING "NFS: v4 server %s "
						"does not accept raw "
						"uid/gids. "
						"Reenabling the idmapper.\n",
						server->nfs_client->cl_hostname);
			}
	}
	/* We failed to handle the error */
	return nfs4_map_errors(ret);
wait_on_recovery:
	exception->recovering = 1;
	return 0;
}