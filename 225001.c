 *
 * Mark a session as ready to accept IO.
 */
void iscsi_unblock_session(struct iscsi_cls_session *session)
{
	queue_work(iscsi_eh_timer_workq, &session->unblock_work);
	/*
	 * Blocking the session can be done from any context so we only
	 * queue the block work. Make sure the unblock work has completed
	 * because it flushes/cancels the other works and updates the state.