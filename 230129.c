	__must_hold(RCU)

{
	DEFINE_WAIT(wait);
	int status = 0;
	for (;;) {

		if (!nfs_need_update_open_stateid(state, stateid))
			return;
		if (!test_bit(NFS_STATE_CHANGE_WAIT, &state->flags))
			break;
		if (status)
			break;
		/* Rely on seqids for serialisation with NFSv4.0 */
		if (!nfs4_has_session(NFS_SERVER(state->inode)->nfs_client))
			break;

		prepare_to_wait(&state->waitq, &wait, TASK_KILLABLE);
		/*
		 * Ensure we process the state changes in the same order
		 * in which the server processed them by delaying the
		 * update of the stateid until we are in sequence.
		 */
		write_sequnlock(&state->seqlock);
		spin_unlock(&state->owner->so_lock);
		rcu_read_unlock();
		trace_nfs4_open_stateid_update_wait(state->inode, stateid, 0);
		if (!signal_pending(current)) {
			if (schedule_timeout(5*HZ) == 0)
				status = -EAGAIN;
			else
				status = 0;
		} else
			status = -EINTR;
		finish_wait(&state->waitq, &wait);
		rcu_read_lock();
		spin_lock(&state->owner->so_lock);
		write_seqlock(&state->seqlock);
	}

	if (test_bit(NFS_OPEN_STATE, &state->flags) &&
	    !nfs4_stateid_match_other(stateid, &state->open_stateid)) {
		nfs4_stateid_copy(freeme, &state->open_stateid);
		nfs_test_and_clear_all_open_stateid(state);
	}

	if (test_bit(NFS_DELEGATED_STATE, &state->flags) == 0)
		nfs4_stateid_copy(&state->stateid, stateid);
	nfs4_stateid_copy(&state->open_stateid, stateid);
	trace_nfs4_open_stateid_update(state->inode, stateid, status);
	nfs_state_log_update_open_stateid(state);
}