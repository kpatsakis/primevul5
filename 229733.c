static int nfs41_sequence_process(struct rpc_task *task,
		struct nfs4_sequence_res *res)
{
	struct nfs4_session *session;
	struct nfs4_slot *slot = res->sr_slot;
	struct nfs_client *clp;
	int status;
	int ret = 1;

	if (slot == NULL)
		goto out_noaction;
	/* don't increment the sequence number if the task wasn't sent */
	if (!RPC_WAS_SENT(task) || slot->seq_done)
		goto out;

	session = slot->table->session;

	trace_nfs4_sequence_done(session, res);

	status = res->sr_status;
	if (task->tk_status == -NFS4ERR_DEADSESSION)
		status = -NFS4ERR_DEADSESSION;

	/* Check the SEQUENCE operation status */
	switch (status) {
	case 0:
		/* Mark this sequence number as having been acked */
		nfs4_slot_sequence_acked(slot, slot->seq_nr);
		/* Update the slot's sequence and clientid lease timer */
		slot->seq_done = 1;
		clp = session->clp;
		do_renew_lease(clp, res->sr_timestamp);
		/* Check sequence flags */
		nfs41_handle_sequence_flag_errors(clp, res->sr_status_flags,
				!!slot->privileged);
		nfs41_update_target_slotid(slot->table, slot, res);
		break;
	case 1:
		/*
		 * sr_status remains 1 if an RPC level error occurred.
		 * The server may or may not have processed the sequence
		 * operation..
		 */
		nfs4_slot_sequence_record_sent(slot, slot->seq_nr);
		slot->seq_done = 1;
		goto out;
	case -NFS4ERR_DELAY:
		/* The server detected a resend of the RPC call and
		 * returned NFS4ERR_DELAY as per Section 2.10.6.2
		 * of RFC5661.
		 */
		dprintk("%s: slot=%u seq=%u: Operation in progress\n",
			__func__,
			slot->slot_nr,
			slot->seq_nr);
		nfs4_slot_sequence_acked(slot, slot->seq_nr);
		goto out_retry;
	case -NFS4ERR_RETRY_UNCACHED_REP:
	case -NFS4ERR_SEQ_FALSE_RETRY:
		/*
		 * The server thinks we tried to replay a request.
		 * Retry the call after bumping the sequence ID.
		 */
		nfs4_slot_sequence_acked(slot, slot->seq_nr);
		goto retry_new_seq;
	case -NFS4ERR_BADSLOT:
		/*
		 * The slot id we used was probably retired. Try again
		 * using a different slot id.
		 */
		if (slot->slot_nr < slot->table->target_highest_slotid)
			goto session_recover;
		goto retry_nowait;
	case -NFS4ERR_SEQ_MISORDERED:
		nfs4_slot_sequence_record_sent(slot, slot->seq_nr);
		/*
		 * Were one or more calls using this slot interrupted?
		 * If the server never received the request, then our
		 * transmitted slot sequence number may be too high.
		 */
		if ((s32)(slot->seq_nr - slot->seq_nr_last_acked) > 1) {
			slot->seq_nr--;
			goto retry_nowait;
		}
		/*
		 * RFC5661:
		 * A retry might be sent while the original request is
		 * still in progress on the replier. The replier SHOULD
		 * deal with the issue by returning NFS4ERR_DELAY as the
		 * reply to SEQUENCE or CB_SEQUENCE operation, but
		 * implementations MAY return NFS4ERR_SEQ_MISORDERED.
		 *
		 * Restart the search after a delay.
		 */
		slot->seq_nr = slot->seq_nr_highest_sent;
		goto out_retry;
	case -NFS4ERR_BADSESSION:
	case -NFS4ERR_DEADSESSION:
	case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
		goto session_recover;
	default:
		/* Just update the slot sequence no. */
		slot->seq_done = 1;
	}
out:
	/* The session may be reset by one of the error handlers. */
	dprintk("%s: Error %d free the slot \n", __func__, res->sr_status);
out_noaction:
	return ret;
session_recover:
	nfs4_schedule_session_recovery(session, status);
	dprintk("%s ERROR: %d Reset session\n", __func__, status);
	nfs41_sequence_free_slot(res);
	goto out;
retry_new_seq:
	++slot->seq_nr;
retry_nowait:
	if (rpc_restart_call_prepare(task)) {
		nfs41_sequence_free_slot(res);
		task->tk_status = 0;
		ret = 0;
	}
	goto out;
out_retry:
	if (!rpc_restart_call(task))
		goto out;
	rpc_delay(task, NFS4_POLL_RETRY_MAX);
	return 0;
}